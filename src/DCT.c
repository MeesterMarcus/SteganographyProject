#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

int sz;
void Compute8x8Dct(double in[8][8], double out[8][8]);
void Compute8x8Idct(double in[8][8], double out[8][8]);
void Print8x8(const char*, double in[8][8]);
void CopyAndManipulate(double ar1[8][8], double ar2[8][8], int*, int);
int *getArr();
/*
This function gets the bit stream generated by the huffman
coding program of the secret message
*/
int *getArr() {
    int i;
    FILE *in;
    in = fopen("msg_stream.txt","r");

    fseek(in, 0, SEEK_END);
    long fsize = ftell(in);
    fseek(in, 0, SEEK_SET);
    char *stream = malloc(fsize + 1);
    fread(stream, fsize, 1, in);
    fclose(in);

    stream[fsize] = 0;
    sz = strlen(stream);

    int *arr;
    arr = (int*) malloc (sizeof(int)*sz);
    for (i = 0; i < sz; i++) {
        arr[i]=((int)*stream++)-48;
    }
    return arr;
}

void Compute8x8Dct(double in[8][8], double out[8][8]) {
    int i, j, u, v;
    double s;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
        {
            s = 0;

            for (u = 0; u < 8; u++)
                for (v = 0; v < 8; v++)
                    s += in[u][v] * cos((2 * u + 1) * i * M_PI / 16) *
                         cos((2 * v + 1) * j * M_PI / 16) *
                         ((i == 0) ? 1 / sqrt(2) : 1) *
                         ((j == 0) ? 1 / sqrt(2) : 1);

            out[i][j] = s / 4;
        }
}

void Compute8x8Idct(double in[8][8], double out[8][8]) {
    int i, j, u, v;
    double s;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
        {
            s = 0;

            for (u = 0; u < 8; u++)
                for (v = 0; v < 8; v++)
                    s += in[u][v] * cos((2 * i + 1) * u * M_PI / 16) *
                         cos((2 * j + 1) * v * M_PI / 16) *
                         ((u == 0) ? 1 / sqrt(2) : 1.) *
                         ((v == 0) ? 1 / sqrt(2) : 1.);

            out[i][j] = s / 4;
        }
}

void Print8x8(const char* title, double in[8][8]) {
    int i, j;

    printf("%s\n", title);
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
            printf("%8.3f ", in[i][j]);
        printf("\n");
    }
}

void CopyAndManipulate
(double ar1[8][8], double ar2[8][8], int array[], int count) {
    int a, b, i=0;
    for (b=0; b<8; b++) {
        for (a=0; a<8; a++) {
            ar2[b][a] = ar1[b][a];
            if (b == 0 && a == 0) {//do nothing
            } else {
                if (i<63) {
                    int num = (int)ar2[b][a];
                    int lsb = num&1;
                    if (lsb == array[count]) {
                    } else if (lsb != array[count]) {
                        if (ar2[b][a] > 0) {
                            ar2[b][a] = ar2[b][a]+1;
                        } else {
                            ar2[b][a] = ar2[b][a]-1;
                        }
                    }
                    i++;
                    count++;
                }
            }
        }
    }
}

void mainDCT(unsigned char pic3[][8][8], int totalbks) {
    int i, j;
    int count=0;
    int *arr = getArr();
    int lsbs[sz];

    if (sz > totalbks*8*8) {
        fprintf (stderr, "bitstream is too big to hide in file\n");
        exit(1);
    }
    int blksalter = ceil(sz/63.0);
    printf("Message stream from file in integer:\n");
    for (i = 0; i < sz; i++) {
        printf("%d",arr[i]);
    }
    printf("\n");
    /*The actual bit stream array in integer*/
    int safety = 0;
    int block = 0;
    do
    {
        double pic1[8][8];
        int a, b;
        for (a =0; a<8; a++) {
            for (b=0; b<8; b++) {
                pic1[a][b] =  (double)pic3[block][a][b];
            }
        }
        double dct[8][8], pic2[8][8], copy[8][8];
        Compute8x8Dct(pic1, dct);
        CopyAndManipulate(dct, copy, arr, safety);
        Compute8x8Idct(copy, pic2);
        Compute8x8Dct(pic2, dct);

        for (i=0; i<8; i++) {
            for (j=0; j<8; j++) {
                if (i == 0 && j == 0) {//skip
                } else {
                    if (count < sz) {
                        int num = (int)dct[i][j];
                        int lsb = num&1;
                        lsbs[count] = lsb;

                        count++;
                    }
                }
            }
        }
        int d, f;
        for (d=0; d<8; d++) {
            for(f=0; f<8; f++) {
                pic3[block][d][f] = (unsigned char)pic2[d][f];
            }
        }
        block++;
        safety = safety+63;
    } while (block < blksalter);
    /* see if the lsbs extracted are the same as arr */
    printf("Message stream extracted from coefficients:\n");
    for (i = 0; i < sz; i++) {
        printf("%d",lsbs[i]);
    }
    for (i=0; i<sz; i++) {
        if(arr[i] != lsbs[i]) printf("\n\nError: Did not match bit stream.\n\n");
        else {
            printf("\nLSB's match huffman bit stream, now decode.\n\n");
            FILE *out;
            out = fopen("lsbstream.txt","w");
            for (i = 0; i < sz; i++) {
                fprintf(out,"%d",lsbs[i]);
            }
            fclose(out);

        }
    }

}

