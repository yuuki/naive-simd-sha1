#include <stdio.h>
#include <string.h>

#include "sha1.h"

#include <sys/time.h>

double mysecond() {
    struct timeval tp;
    struct timezone tzp;
    int i;

    i = gettimeofday(&tp,&tzp);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);
}

void pt(unsigned char md[VECTOR_SIZE][SHA_DIGEST_LENGTH]) {
    int i, j;

    for (j = 0; j < VECTOR_SIZE; j++) {
        for (i = 0; i < SHA_DIGEST_LENGTH; i++)
            printf("%02x", md[j][i]);
        printf("\n");
    }
}

void do_fp(FILE *f) {
    sha1_context ctx[VECTOR_SIZE];
    unsigned char md[VECTOR_SIZE][SHA_DIGEST_LENGTH];
    uint32        msglen[VECTOR_SIZE];
    int fd;
    int i, j;
    double t;
    unsigned char buf[VECTOR_SIZE][BUFSIZE];
    unsigned char buff[BUFSIZE];

    fd = fileno(f);
    i = read(fd, buff, BUFSIZE);
    for (j = 0; j < VECTOR_SIZE; j++) {
        msglen[j] = i;
        memcpy(buf[j], buff, (unsigned long)i);
    }
    if (i <= 0) return;
    t = mysecond();
    sse_sha1_init(ctx);
    sse_sha1_update(ctx, buf, msglen);
    sse_sha1_final(ctx, md);
    printf("time on memory is %11.4f\n", mysecond() - t);
    printf("Input size is %d\n", i);
    pt(md);
    return;
}

int main(int argc, char **argv) {
    int i, err = 0;
    FILE *IN;

    if (argc == 1) {
        do_fp(stdin);
    } else {
        for (i = 1; i < argc; i++) {
            IN=fopen(argv[i],"r");
            if (IN == NULL) {
                perror(argv[i]);
                err++;
                continue;
            }
            printf("SHA1(%s)= ",argv[i]);
            do_fp(IN);
            fclose(IN);
        }
    }
    return err;
}

