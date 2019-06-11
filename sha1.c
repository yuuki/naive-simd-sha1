#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha1.h"

#define INIT_DATA_h0 0x67452301UL
#define INIT_DATA_h1 0xefcdab89UL
#define INIT_DATA_h2 0x98badcfeUL
#define INIT_DATA_h3 0x10325476UL
#define INIT_DATA_h4 0xc3d2e1f0UL

#define HOST_c2l(c,l)	({ unsigned int r=*((const unsigned int *)(c));	\
				   asm ("bswapl %0":"=r"(r):"0"(r));	\
				   (c)+=4; (l)=r;			})
#define HOST_l2c(l,c)	({ unsigned int r=(l);			\
				   asm ("bswapl %0":"=r"(r):"0"(r));	\
				   *((unsigned int *)(c))=r; (c)+=4; r;	})

#define GET_UINT32(n,b,i)                       \
{                                               \
    (n) = ( (uint32) (b)[(i)    ] << 24 )       \
    | ( (uint32) (b)[(i) + 1] << 16 )       \
    | ( (uint32) (b)[(i) + 2] <<  8 )       \
    | ( (uint32) (b)[(i) + 3]       );      \
}

#define PUT_UINT32(n,b,i)                       \
{                                               \
    (b)[(i)    ] = (uint8) ( (n) >> 24 );       \
    (b)[(i) + 1] = (uint8) ( (n) >> 16 );       \
    (b)[(i) + 2] = (uint8) ( (n) >>  8 );       \
    (b)[(i) + 3] = (uint8) ( (n)       );       \
}

#define S(_x, n, _data)                     \
    (_x) = (_mm_or_si128(_mm_slli_epi32(_data, n), _mm_srli_epi32(_data, 32-n)));


void sse_sha1_init(sha1_context ctx[]) {
    int i;

    for (i = 0; i < VECTOR_SIZE; i++) {
        ctx[i].Nh = 0;
        ctx[i].Nl = 0;
        ctx[i].num = 0;

        ctx[i].hash[0] = INIT_DATA_h0;
        ctx[i].hash[1] = INIT_DATA_h1;
        ctx[i].hash[2] = INIT_DATA_h2;
        ctx[i].hash[3] = INIT_DATA_h3;
        ctx[i].hash[4] = INIT_DATA_h4;
    }
}

#define GEN_WORD(xmm0, xmm1, W, t)        \
    (xmm0) = _mm_load_si128((__m128i *)(W)[(t) - 3]);   \
    (xmm1) = _mm_load_si128((__m128i *)(W)[(t) - 8]);   \
    (xmm1) = _mm_xor_si128((xmm0), (xmm1));           \
    (xmm0) = _mm_load_si128((__m128i *)(W)[(t) - 14]);  \
    (xmm1) = _mm_xor_si128(xmm1, xmm0);           \
    (xmm0) = _mm_load_si128((__m128i *)(W)[(t) - 16]);  \
    (xmm1) = _mm_xor_si128((xmm1), (xmm0));           \
    S((xmm0), 1, (xmm1));                           \
    _mm_store_si128((__m128i *)(W)[(t)], (xmm0));     \


static void generate_words(uint32 W[80][VECTOR_SIZE]) {
    __m128i xmm0, xmm1;

    GEN_WORD(xmm0, xmm1, W, 16);
    GEN_WORD(xmm0, xmm1, W, 17);
    GEN_WORD(xmm0, xmm1, W, 18);
    GEN_WORD(xmm0, xmm1, W, 19);
    GEN_WORD(xmm0, xmm1, W, 20);
    GEN_WORD(xmm0, xmm1, W, 21);
    GEN_WORD(xmm0, xmm1, W, 22);
    GEN_WORD(xmm0, xmm1, W, 23);
    GEN_WORD(xmm0, xmm1, W, 24);
    GEN_WORD(xmm0, xmm1, W, 25);
    GEN_WORD(xmm0, xmm1, W, 26);
    GEN_WORD(xmm0, xmm1, W, 27);
    GEN_WORD(xmm0, xmm1, W, 28);
    GEN_WORD(xmm0, xmm1, W, 29);
    GEN_WORD(xmm0, xmm1, W, 30);
    GEN_WORD(xmm0, xmm1, W, 31);
    GEN_WORD(xmm0, xmm1, W, 32);
    GEN_WORD(xmm0, xmm1, W, 33);
    GEN_WORD(xmm0, xmm1, W, 34);
    GEN_WORD(xmm0, xmm1, W, 35);
    GEN_WORD(xmm0, xmm1, W, 36);
    GEN_WORD(xmm0, xmm1, W, 37);
    GEN_WORD(xmm0, xmm1, W, 38);
    GEN_WORD(xmm0, xmm1, W, 39);
    GEN_WORD(xmm0, xmm1, W, 40);
    GEN_WORD(xmm0, xmm1, W, 41);
    GEN_WORD(xmm0, xmm1, W, 42);
    GEN_WORD(xmm0, xmm1, W, 43);
    GEN_WORD(xmm0, xmm1, W, 44);
    GEN_WORD(xmm0, xmm1, W, 45);
    GEN_WORD(xmm0, xmm1, W, 46);
    GEN_WORD(xmm0, xmm1, W, 47);
    GEN_WORD(xmm0, xmm1, W, 48);
    GEN_WORD(xmm0, xmm1, W, 49);
    GEN_WORD(xmm0, xmm1, W, 50);
    GEN_WORD(xmm0, xmm1, W, 51);
    GEN_WORD(xmm0, xmm1, W, 52);
    GEN_WORD(xmm0, xmm1, W, 53);
    GEN_WORD(xmm0, xmm1, W, 54);
    GEN_WORD(xmm0, xmm1, W, 55);
    GEN_WORD(xmm0, xmm1, W, 56);
    GEN_WORD(xmm0, xmm1, W, 57);
    GEN_WORD(xmm0, xmm1, W, 58);
    GEN_WORD(xmm0, xmm1, W, 59);
    GEN_WORD(xmm0, xmm1, W, 60);
    GEN_WORD(xmm0, xmm1, W, 61);
    GEN_WORD(xmm0, xmm1, W, 62);
    GEN_WORD(xmm0, xmm1, W, 63);
    GEN_WORD(xmm0, xmm1, W, 64);
    GEN_WORD(xmm0, xmm1, W, 65);
    GEN_WORD(xmm0, xmm1, W, 66);
    GEN_WORD(xmm0, xmm1, W, 67);
    GEN_WORD(xmm0, xmm1, W, 68);
    GEN_WORD(xmm0, xmm1, W, 69);
    GEN_WORD(xmm0, xmm1, W, 70);
    GEN_WORD(xmm0, xmm1, W, 71);
    GEN_WORD(xmm0, xmm1, W, 72);
    GEN_WORD(xmm0, xmm1, W, 73);
    GEN_WORD(xmm0, xmm1, W, 74);
    GEN_WORD(xmm0, xmm1, W, 75);
    GEN_WORD(xmm0, xmm1, W, 76);
    GEN_WORD(xmm0, xmm1, W, 77);
    GEN_WORD(xmm0, xmm1, W, 78);
    GEN_WORD(xmm0, xmm1, W, 79);
}

/* Constants defined in SHA-1   */
#define K_00_19	0x5a827999UL
#define K_20_39 0x6ed9eba1UL
#define K_40_59 0x8f1bbcdcUL
#define K_60_79 0xca62c1d6UL

#define F_00_19(xmm1, xmm2, _b, _c, _d)     \
    (xmm1) = _mm_and_si128((_b), (_c));        \
    (xmm2) = _mm_andnot_si128((_b), (_d));     \
    (xmm2) = _mm_or_si128(xmm1, xmm2);     \

#define F_20_39(xmm1, xmm2, _b, _c, _d)   \
    (xmm1) = _mm_xor_si128((_b), (_c)); \
    (xmm2) = _mm_xor_si128((xmm1), (_d));          \

#define F_40_59(xmm1, xmm2, xmm3, _b, _c, _d)  \
    (xmm1) = _mm_and_si128((_b), (_c));       \
    (xmm2) = _mm_and_si128((_b), (_d));       \
    (xmm3) = _mm_and_si128((_c), (_d));       \
    (xmm2) = _mm_or_si128((xmm1), (xmm2));    \
    (xmm2) = _mm_or_si128((xmm2), (xmm3));    \

#define F_60_79(xmm1, xmm2, _b, _c, _d) F_20_39(xmm1, xmm2, _b, _c, _d)


static void sha1_process(sha1_context ctx[], const uint8 *data[VECTOR_SIZE], size_t num) {
    int i, t;                          /* Loop Counter */
    __m128i _temp;
    __attribute__((aligned(16))) uint32 W[80][VECTOR_SIZE];   /* Word Sequence */
    __m128i _A, _B, _C, _D, _E, _W, _K;
    __m128i xmm0, xmm1, xmm2, xmm3;

    while(1) {
        /* initilize word array */
        for (i = 0; i < VECTOR_SIZE; i++) {
            for (t = 0; t < 16; t++) {
                W[t][i] = data[i][t * 4] << 24;
                W[t][i] |= data[i][t * 4 + 1] << 16;
                W[t][i] |= data[i][t * 4 + 2] << 8;
                W[t][i] |= data[i][t * 4 + 3];
            }
            data[i] += SHA_CBLOCK;
        }

        generate_words(W);

        _A = _mm_set_epi32(ctx[3].hash[0], ctx[2].hash[0], ctx[1].hash[0], ctx[0].hash[0]);
        _B = _mm_set_epi32(ctx[3].hash[1], ctx[2].hash[1], ctx[1].hash[1], ctx[0].hash[1]);
        _C = _mm_set_epi32(ctx[3].hash[2], ctx[2].hash[2], ctx[1].hash[2], ctx[0].hash[2]);
        _D = _mm_set_epi32(ctx[3].hash[3], ctx[2].hash[3], ctx[1].hash[3], ctx[0].hash[3]);
        _E = _mm_set_epi32(ctx[3].hash[4], ctx[2].hash[4], ctx[1].hash[4], ctx[0].hash[4]);

        _K = _mm_set_epi32(K_00_19, K_00_19, K_00_19, K_00_19);
        for (t = 0; t < 20; t++) {
            _W = _mm_load_si128((__m128i *)W[t]);

            S(xmm0, 5, _A);
            F_00_19(xmm1, xmm2, _B, _C, _D);
            xmm2 = _mm_add_epi32(xmm0, xmm2);
            xmm2 = _mm_add_epi32(xmm2, _E);
            xmm2 = _mm_add_epi32(xmm2, _W);
            _temp = _mm_add_epi32(xmm2, _K);
            _E = _D;
            _D = _C;
            S(_C, 30, _B);
            _B = _A;
            _A = _temp;
        }

        _K = _mm_set_epi32(K_20_39, K_20_39, K_20_39, K_20_39);
        for (t = 20; t < 40; t++) {
            _W = _mm_load_si128((__m128i *)W[t]);

            S(xmm0, 5, _A);
            F_20_39(xmm1, xmm2, _B, _C, _D);
            xmm2 = _mm_add_epi32(xmm0, xmm2);
            xmm2 = _mm_add_epi32(xmm2, _E);
            xmm2 = _mm_add_epi32(xmm2, _W);
            _temp = _mm_add_epi32(xmm2, _K);
            _E = _D;
            _D = _C;
            S(_C, 30, _B);
            _B = _A;
            _A = _temp;
        }

        _K = _mm_set_epi32(K_40_59, K_40_59, K_40_59, K_40_59);
        for (t = 40; t < 60; t++) {
            _W = _mm_load_si128((__m128i *)W[t]);

            S(xmm0, 5, _A);
            F_40_59(xmm1, xmm2, xmm3, _B, _C, _D);
            xmm2 = _mm_add_epi32(xmm0, xmm2);
            xmm2 = _mm_add_epi32(xmm2, _E);
            xmm2 = _mm_add_epi32(xmm2, _W);
            _temp = _mm_add_epi32(xmm2, _K);
            _E = _D;
            _D = _C;
            S(_C, 30, _B);
            _B = _A;
            _A = _temp;
        }

        _K = _mm_set_epi32(K_60_79, K_60_79, K_60_79, K_60_79);
        for (t = 60; t < 80; t++) {
            _W = _mm_load_si128((__m128i *)W[t]);

            S(xmm0, 5, _A);
            F_60_79(xmm1, xmm2, _B, _C, _D);
            xmm2 = _mm_add_epi32(xmm0, xmm2);
            xmm2 = _mm_add_epi32(xmm2, _E);
            xmm2 = _mm_add_epi32(xmm2, _W);
            _temp = _mm_add_epi32(xmm2, _K);
            _E = _D;
            _D = _C;
            S(_C, 30, _B);
            _B = _A;
            _A = _temp;
        }

        ctx[0].hash[0] += _mm_extract_epi32(_A, 0);
        ctx[0].hash[1] += _mm_extract_epi32(_B, 0);
        ctx[0].hash[2] += _mm_extract_epi32(_C, 0);
        ctx[0].hash[3] += _mm_extract_epi32(_D, 0);
        ctx[0].hash[4] += _mm_extract_epi32(_E, 0);

        ctx[1].hash[0] += _mm_extract_epi32(_A, 1);
        ctx[1].hash[1] += _mm_extract_epi32(_B, 1);
        ctx[1].hash[2] += _mm_extract_epi32(_C, 1);
        ctx[1].hash[3] += _mm_extract_epi32(_D, 1);
        ctx[1].hash[4] += _mm_extract_epi32(_E, 1);

        ctx[2].hash[0] += _mm_extract_epi32(_A, 2);
        ctx[2].hash[1] += _mm_extract_epi32(_B, 2);
        ctx[2].hash[2] += _mm_extract_epi32(_C, 2);
        ctx[2].hash[3] += _mm_extract_epi32(_D, 2);
        ctx[2].hash[4] += _mm_extract_epi32(_E, 2);

        ctx[3].hash[0] += _mm_extract_epi32(_A, 3);
        ctx[3].hash[1] += _mm_extract_epi32(_B, 3);
        ctx[3].hash[2] += _mm_extract_epi32(_C, 3);
        ctx[3].hash[3] += _mm_extract_epi32(_D, 3);
        ctx[3].hash[4] += _mm_extract_epi32(_E, 3);

        if (--num == 0) break;
    }
}

int sse_sha1_update(sha1_context ctx[], const unsigned char data_[VECTOR_SIZE][BUFSIZE], uint32 length[]) {
    int i, j = 0;
	const uint8 *data[VECTOR_SIZE];
	uint8 *p[VECTOR_SIZE];
	uint32 l[VECTOR_SIZE];
	size_t n[VECTOR_SIZE];
    // TODO: lengthは４入力同じ長さと仮定
    // lengthの長さが小さいものにはフラグを立てておく
    //for (i = 0; i < VECTOR_SIZE; i++) {
        //if (length[i] == 0) {
            //return 1;
        //}
    //}

    for (i = 0; i < VECTOR_SIZE; i++) {
        l[i] = (ctx[i].Nl + (((uint32)length[i]) << 3)) & 0xffffffffUL;
        if (l[i] < ctx[i].Nl) { /* overflow */
            ctx[i].Nh++;
        }
        ctx[i].Nh += (uint32)(length[i] >> 29);	/* might cause compiler warning on 16-bit */
        ctx[i].Nl = l[i];

        n[i] = ctx[i].num;
        data[i] = data_[i];
    }
    if (n[0] != 0) {
        for (i = 0; i < VECTOR_SIZE; i++) {
            p[i] = (uint8 *)ctx[i].data;
        }

        if (length[0] >= SHA_CBLOCK || (length[0] + n[0]) >= SHA_CBLOCK) {
            for (i = 0; i < VECTOR_SIZE; i++) {
                memcpy(p[i] + n[i], data[i], SHA_CBLOCK - n[i]);
            }
            sha1_process(ctx, p, 1);
            for (i = 0; i < VECTOR_SIZE; i++) {
                n[i]        = SHA_CBLOCK - n[i];
                data[i]     += n[i];
                length[i]   -= n[i];
                ctx[i].num    = 0;
                memset(p[i], 0, SHA_CBLOCK);	/* keep it zeroed */
            }
        } else {
            for (i = 0; i < VECTOR_SIZE; i++) {
                memcpy(p[i] + n[i], data[i], length[i]);
                ctx[i].num += (uint32)length[i];
            }
            return 1;
        }
    }

    for (i = 0; i < VECTOR_SIZE; i++) {
        n[i] = length[i]/SHA_CBLOCK;
    }
    if (n[0] > 0) {
        sha1_process(ctx, data, n[0]);
        for (i = 0; i < VECTOR_SIZE; i++) {
            n[i]        *= SHA_CBLOCK;
            data[i]     += n[i];
            length[i]   -= n[i];
        }
    }

    if (length[0] != 0) {
        for (i = 0; i < VECTOR_SIZE; i++) {
            p[i] = (uint8 *)ctx[i].data;
            ctx[i].num = (uint32)length[i];
            memcpy(p[i], data[i], length[i]);
        }
    }

    return 1;
}

int sse_sha1_final(sha1_context ctx[], uint8 digest[][20]) {
    int i;
	uint8 *p[VECTOR_SIZE];
	size_t n[VECTOR_SIZE];

    for (i = 0; i < VECTOR_SIZE; i++) {
        p[i] = ctx[i].data;
        n[i] = ctx[i].num;
        p[i][n[i]] = 0x80; /* there is always room for one */
        n[i]++;
    }

	if (n[0] > (SHA_CBLOCK - 8)) {
        for (i = 0; i < VECTOR_SIZE; i++) {
            memset(p[i] + n[i], 0, SHA_CBLOCK - n[i]);
            n[i] = 0;
            sha1_process(ctx, p, 1);
        }
    }
    for (i = 0; i < VECTOR_SIZE; i++) {
        memset(p[i] + n[i], 0, SHA_CBLOCK - 8 - n[i]);

        p[i] += SHA_CBLOCK - 8;
        (void)HOST_l2c(ctx[i].Nh, p[i]);
        (void)HOST_l2c(ctx[i].Nl, p[i]);

        p[i] -= SHA_CBLOCK;
    }
	sha1_process(ctx, p, 1);

    for (i = 0; i < VECTOR_SIZE; i++) {
        PUT_UINT32(ctx[i].hash[0], digest[i],  0);
        PUT_UINT32(ctx[i].hash[1], digest[i],  4);
        PUT_UINT32(ctx[i].hash[2], digest[i],  8);
        PUT_UINT32(ctx[i].hash[3], digest[i], 12);
        PUT_UINT32(ctx[i].hash[4], digest[i], 16);
    }
    for (i = 0; i < VECTOR_SIZE; i++) {
        ctx[i].num = 0;
    }
    return 1;
}
