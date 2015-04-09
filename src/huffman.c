#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node_t {
    struct node_t *left, *right;
    int freq;
    char c;
} *node;

struct node_t pool[256] = {{0}};
node qqq[255], *q = qqq - 1;
int n_nodes = 0, qend = 1;
char *code[128] = {0}, buf[2560], buf2[2560];
int *getArr(char *stream);
int *streamarr;
int sz;
node new_node(int freq, char c, node a, node b)
{
    node n = pool + n_nodes++;
    if (freq) n->c = c, n->freq = freq;
    else {
        n->left = a, n->right = b;
        n->freq = a->freq + b->freq;
    }
    return n;
}

/* priority queue */
void qinsert(node n)
{
    int j, i = qend++;
    while ((j = i / 2)) {
        if (q[j]->freq <= n->freq) break;
        q[i] = q[j], i = j;
    }
    q[i] = n;
}

node qremove()
{
    int i, l;
    node n = q[i = 1];

    if (qend < 2) return 0;
    qend--;
    while ((l = i * 2) < qend) {
        if (l + 1 < qend && q[l + 1]->freq < q[l]->freq) l++;
        q[i] = q[l], i = l;
    }
    q[i] = q[qend];
    return n;
}

/* walk the tree and put 0s and 1s */
void build_code(node n, char *s, int len)
{
    static char *out = buf;
    if (n->c) {
        s[len] = 0;
        strcpy(out, s);
        code[n->c] = out;
        out += len + 1;
        return;
    }

    s[len] = '0';
    build_code(n->left,  s, len + 1);
    s[len] = '1';
    build_code(n->right, s, len + 1);
}

void init(const char *s)
{
    int i, freq[128] = {0};
    char c[16];

    while (*s) freq[(int)*s++]++;

    for (i = 0; i < 128; i++)
        if (freq[i]) qinsert(new_node(freq[i], i, 0, 0));

    while (qend > 2)
        qinsert(new_node(0, 0, qremove(), qremove()));

    build_code(q[1], c, 0);
}

void encode(const char *s, char *out)
{
    while (*s) {
        strcpy(out, code[*s]);
        out += strlen(code[*s++]);
    }
}

void decode(const char *s, node t)
{
    node n = t;
    while (*s) {
        if (*s++ == '0') n = n->left;
        else n = n->right;

        if (n->c) putchar(n->c), n = t;
    }
    if (t != n) printf("garbage input\n");
}

int *getArr(char *stream) {
    int i;
    for (i = 0; i < sz; i++) {
        streamarr[i]=((int)*stream++)-48;

    }
    return streamarr;
}

int main(int argc, char **argv) {
    int i;
    FILE *in;
    in = fopen("msg.txt","r");

    fseek(in, 0, SEEK_END);
    long fsize = ftell(in);
    fseek(in, 0, SEEK_SET);
    char *stream = malloc(fsize + 1);
    fread(stream, fsize, 1, in);
    fclose(in);
    stream[fsize] = 0;
    const char *str = stream, buf[2560];
    init(str);

    if (argc == 1) {
        printf("Usage: ./huffman [-e][-d]\n");
    }


    else if(strcmp(argv[1],"-e")==0) {
        for (i = 0; i < 128; i++)
            if (code[i]) printf("'%c': %s\n", i, code[i]);

        encode(str, buf);
        printf("encoded: %s\n", buf);

        //Print the message stream to a file
        FILE *out;
        out = fopen("msg_stream.txt","w");
        fprintf(out,"%s",buf);
        fclose(out);

        sz = strlen(buf);
        printf("Size of encoded bit stream in bits: %d\n",sz);
    } else if (strcmp(argv[1],"-d")==0) {

        FILE *in2;
        in2 = fopen("lsbstream.txt","rb");
        fseek(in2, 0, SEEK_END);
        long fsize2 = ftell(in2);
        fseek(in2, 0, SEEK_SET);
        char *dstream = malloc(fsize2 + 1);
        fread(dstream, fsize2, 1, in2);
        fclose(in2);
        dstream[fsize2] = 0;
        const char *str2 = dstream, buf2[2560];
        printf("decoded: ");
        decode(str2, q[1]);
    } else {
        printf("Error: Please enter proper input.\n");
    }
    return 0;
}
