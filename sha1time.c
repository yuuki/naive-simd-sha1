#include <stdio.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <sys/time.h>

#include "sha1.h"


#define HLINE "-------------------------------------------------------------\n"

#define NTIMES	1000

#ifndef MIN
#   define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#   define MAX(x,y) ((x)>(y)?(x):(y))
#endif

static double   sumtime = 0, avgtime = 0, maxtime = 0, mintime = FLT_MAX;

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
    sha1_context    ctx[VECTOR_SIZE];
    unsigned char   md[VECTOR_SIZE][SHA_DIGEST_LENGTH];
    uint32          msglen[VECTOR_SIZE];
    int             fd;
    int             i, j, k;
    unsigned char   buf[VECTOR_SIZE][BUFSIZE];
    double		    times[NTIMES];

    fd = fileno(f);

    for (k = 0; k < NTIMES; k++) {
        for (j = 0; j < VECTOR_SIZE; j++) {
            msglen[j] = read(fd, buf[j], BUFSIZE);
            if (msglen[j] == 0) goto END;
            printf("Input size is %dB\n", msglen[j]);
        }
        memset(md, 0, sizeof(unsigned char) * VECTOR_SIZE * SHA_DIGEST_LENGTH);

        times[k] = mysecond(); //計測開始

        sse_sha1_init(ctx);
        sse_sha1_update(ctx, buf, msglen);
        sse_sha1_final(ctx, md);

        times[k] = mysecond() - times[k]; //計測終了
        pt(md);
    }
END:
    for (i = 0; i < k; i++) {
        avgtime = avgtime + times[i];
        mintime = MIN(mintime, times[i]);
        maxtime = MAX(maxtime, times[i]);
//      printf("%11.4f\n", times[i]);
    }
    printf("The number of test is %d times\n", k);
    printf("Sumtime     Avg time     Min time     Max time  Throughput\n");
    sumtime = avgtime;
    avgtime = avgtime / (double)(k);
    printf("%11.4f %11.4f  %11.4f  %11.4f %11fMB/s\n", sumtime, avgtime, mintime, maxtime, ((BUFSIZE * k)/sumtime/1000000));
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
            do_fp(IN);
            fclose(IN);
        }
    }
    return err;
}
