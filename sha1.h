#ifndef _SHA1_H
#define _SHA1_H

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

/* Error Code */
#define SHA1_OK             0
#define SHA1_NULL           -1  /* Null Pointer Parameter */
#define SHA1_INPUT_TOOLONG  -2  /* input data too long */
#define SHA1_STATE_ERROR    -3  /* called Input after Result */

#define VECTOR_SIZE        4   /* 4 parallel per core by SSE */

#define SHA_CBLOCK         64
#define SHA_DIGEST_LENGTH   20
#define BUFSIZE	            1000*64

#ifndef _OSD_POSIX
int read(int, void *, unsigned int);
#endif


typedef struct {
    uint32 hash[SHA_DIGEST_LENGTH/4]; /* Message Digest */
    uint32 Nl, Nh;  /* low, high */

    uint8 data[64]; /* 512 bit message block */
    int num;
} sha1_context;

void sse_sha1_init(sha1_context ctx[]);
int sse_sha1_update(sha1_context ctx[], const unsigned char data_[VECTOR_SIZE][BUFSIZE], uint32 length[]);
int sse_sha1_final(sha1_context ctx[], uint8 digest[][20]);

#endif /* sha1.h */
