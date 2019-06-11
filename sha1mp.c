#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <sys/time.h>

#include "sha1.h"


#define HLINE "-------------------------------------------------------------\n"

#define NTIMES 1666	

#ifndef MIN
#   define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#   define MAX(x,y) ((x)>(y)?(x):(y))
#endif

static double   sumtime = 0, avgtime = 0, maxtime = 0, mintime = FLT_MAX;
static int      num_threads = 1;

#ifdef _OPENMP
extern int omp_get_num_threads();
#endif
 
double mysecond() {
    struct timeval tp;
    struct timezone tzp;
    int i;

    i = gettimeofday(&tp,&tzp);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);
}

void pt(unsigned char md[VECTOR_SIZE][SHA_DIGEST_LENGTH]) {
    int i, j;

    for (j = 0; j < VECTOR_SIZE*num_threads; j++) {
        for (i = 0; i < SHA_DIGEST_LENGTH; i++)
            printf("%02x", md[j][i]);
        printf("\n");
    }
}

void do_fp(FILE *f) {
    sha1_context    ctx[num_threads][VECTOR_SIZE];
    uint32          msglen[num_threads][VECTOR_SIZE];
    int             fd;
    int             i, j, k;
    double		    times[NTIMES];
    fd = fileno(f);
    unsigned char   md[VECTOR_SIZE*num_threads][SHA_DIGEST_LENGTH];
    unsigned char   buf[VECTOR_SIZE*num_threads][BUFSIZE];

    memset(md, 0, sizeof(unsigned char) * num_threads * VECTOR_SIZE * SHA_DIGEST_LENGTH);
    for (k = 0; k < NTIMES; k++) {
        for (j = 0; j < num_threads; j++) {
            for (i = 0; i < VECTOR_SIZE; i++) {
                msglen[j][i] = read(fd, buf[j*VECTOR_SIZE + i], BUFSIZE);
                if (msglen[j][i] == 0) goto END;
                //printf("Input size is %dB\n", msglen[j]);
            }
        }

        times[k] = mysecond(); //計測開始

#pragma omp parallel for 
        for (j = 0; j < num_threads; j++) {
            sse_sha1_init(ctx[j]);
            sse_sha1_update(ctx[j], &(buf[j * VECTOR_SIZE]), &(msglen[j][0]));
            sse_sha1_final(ctx[j], &(md[j * VECTOR_SIZE]));
        }

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
    printf("Number of Threads requested = %i\n", num_threads);
    printf("Sumtime     Avg time     Min time     Max time  Throughput\n");
    sumtime = avgtime;
    avgtime = avgtime / (double)(k);
    printf("%11.4f %11.4f  %11.4f  %11.4f %11.4f GB/s\n", sumtime, avgtime, mintime, maxtime, ((BUFSIZE * k * num_threads * VECTOR_SIZE)/sumtime/1000000000));
}

int main(int argc, char **argv) {
    int i, err = 0;
    FILE *IN;

#pragma omp parallel 
    {   
#pragma omp master
        {   
            num_threads = omp_get_num_threads();
        } 
    } 

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
            //            printf("SHA1(%s)= ",argv[i]);
            do_fp(IN);
            fclose(IN);
        }
    }
    return err;
}
