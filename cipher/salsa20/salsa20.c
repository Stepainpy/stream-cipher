#define STMCPHR_USE_ROTL32 salsa20i_rotl

#include "salsa20.h"
#include <string.h>
#include "config.h"

typedef stmcphr_u8_t  salsa20_byte_t;
typedef stmcphr_u32_t salsa20_word_t;

static struct {
    salsa20_byte_t gamma[64];
    salsa20_word_t key[8];
    salsa20_word_t nonce[2];
    salsa20_word_t block[2];
    salsa20_byte_t remained;
} salsa20i_ctx;

int salsa20_setup_key(const void* key, int bits) {
    if (bits != 256) return 1;
    memcpy(salsa20i_ctx.key, key, sizeof salsa20i_ctx.key);
    STMCPHR_IF_BIG(STMCPHR_BSWAP_32x8(salsa20i_ctx.key));
    return 0;
}

int salsa20_setup_nonce(const void* nonce, int bits) {
    if (bits != 64) return 1;
    memcpy(salsa20i_ctx.nonce, nonce, sizeof salsa20i_ctx.nonce);
    STMCPHR_IF_BIG(STMCPHR_BSWAP_32x2(salsa20i_ctx.nonce));
    return 0;
}

int salsa20_setup_block(size_t number) {
    memset(salsa20i_ctx.block, 0, sizeof salsa20i_ctx.block);
    memcpy(salsa20i_ctx.block, &number, sizeof number);
    STMCPHR_IF_BIG(STMCPHR_BSWAP_32x2(salsa20i_ctx.block));
    return 0;
}

#define salsa20i_quarterround(M, a, b, c, d) do { \
    M[b] ^= salsa20i_rotl(M[a] + M[d],  7); \
	M[c] ^= salsa20i_rotl(M[b] + M[a],  9); \
	M[d] ^= salsa20i_rotl(M[c] + M[b], 13); \
	M[a] ^= salsa20i_rotl(M[d] + M[c], 18); \
} while (0)

void salsa20_begin_gen(void) {
    salsa20_word_t M[16]; int i;

    M[ 0] = 0x61707865; M[ 5] = 0x3320646e;
    M[10] = 0x79622d32; M[15] = 0x6b206574;

    memcpy(M +  1, salsa20i_ctx.key    , sizeof(salsa20_word_t) * 4);
    memcpy(M +  6, salsa20i_ctx.nonce  , sizeof(salsa20_word_t) * 2);
    memcpy(M +  8, salsa20i_ctx.block  , sizeof(salsa20_word_t) * 2);
    memcpy(M + 11, salsa20i_ctx.key + 4, sizeof(salsa20_word_t) * 4);

    for (i = 0; i < 10; i++) {
        salsa20i_quarterround(M,  0,  4,  8, 12);
        salsa20i_quarterround(M,  5,  9, 13,  1);
        salsa20i_quarterround(M, 10, 14,  2,  6);
        salsa20i_quarterround(M, 15,  3,  7, 11);
        salsa20i_quarterround(M,  0,  1,  2,  3);
        salsa20i_quarterround(M,  5,  6,  7,  4);
        salsa20i_quarterround(M, 10, 11,  8,  9);
        salsa20i_quarterround(M, 15, 12, 13, 14);
    }

    M[ 0] += 0x61707865; M[ 5] += 0x3320646e;
    M[10] += 0x79622d32; M[15] += 0x6b206574;

    for (i = 0; i < 4; i++) M[i +  1] += salsa20i_ctx.key[i];
    for (i = 0; i < 2; i++) M[i +  6] += salsa20i_ctx.nonce[i];
    for (i = 0; i < 2; i++) M[i +  8] += salsa20i_ctx.block[i];
    for (i = 0; i < 4; i++) M[i + 11] += salsa20i_ctx.key[i + 4];

    salsa20i_ctx.remained = sizeof salsa20i_ctx.gamma;
    for (i = 0; i < 16; i++) {
        STMCPHR_IF_BIG(STMCPHR_BSWAP_32_ONE(M[i]));
        memcpy(salsa20i_ctx.gamma + 4 * i, M + i, 4);
    }
}

void salsa20_take_gamma(void* dest, size_t count) {
    salsa20_byte_t* dst = dest; size_t min;
    while (count > 0) {
        min = count < salsa20i_ctx.remained
            ? count : salsa20i_ctx.remained;
        memcpy(dst, salsa20i_ctx.gamma +
            sizeof salsa20i_ctx.gamma - salsa20i_ctx.remained, min);
        dst += min; count -= min; salsa20i_ctx.remained -= min;

        if (salsa20i_ctx.remained == 0) {
            if (++salsa20i_ctx.block[0] == 0) ++salsa20i_ctx.block[1];
            salsa20_begin_gen();
        }
    }
}

void salsa20_skip_gamma(size_t count) {
    size_t min;
    while (count > 0) {
        min = count < salsa20i_ctx.remained
            ? count : salsa20i_ctx.remained;
        salsa20i_ctx.remained -= min;
        count -= min;

        if (salsa20i_ctx.remained == 0) {
            if (++salsa20i_ctx.block[0] == 0) ++salsa20i_ctx.block[1];
            salsa20_begin_gen();
        }
    }
}