#define STMCPHR_USE_ROTL32 chacha20i_rotl

#include "chacha20.h"
#include <string.h>
#include "config.h"

typedef stmcphr_u8_t  chacha20_byte_t;
typedef stmcphr_u32_t chacha20_word_t;

static struct {
    chacha20_byte_t gamma[64];
    chacha20_word_t key[8];
    chacha20_word_t block;
    chacha20_word_t nonce[3];
    chacha20_byte_t remained;
} chacha20i_ctx;

int chacha20_setup_key(const void* key, int bits) {
    if (bits != 256) return 1;

    memcpy(chacha20i_ctx.key, key, sizeof chacha20i_ctx.key);
#if STMCPHR_IS_BIG
    chacha20i_ctx.key[0] = stmcphr_bswap32(chacha20i_ctx.key[0]);
    chacha20i_ctx.key[1] = stmcphr_bswap32(chacha20i_ctx.key[1]);
    chacha20i_ctx.key[2] = stmcphr_bswap32(chacha20i_ctx.key[2]);
    chacha20i_ctx.key[3] = stmcphr_bswap32(chacha20i_ctx.key[3]);
    chacha20i_ctx.key[4] = stmcphr_bswap32(chacha20i_ctx.key[4]);
    chacha20i_ctx.key[5] = stmcphr_bswap32(chacha20i_ctx.key[5]);
    chacha20i_ctx.key[6] = stmcphr_bswap32(chacha20i_ctx.key[6]);
    chacha20i_ctx.key[7] = stmcphr_bswap32(chacha20i_ctx.key[7]);
#endif

    return 0;
}

int chacha20_setup_nonce(const void* nonce, int bits) {
    if (bits != 96) return 1;

    memcpy(chacha20i_ctx.nonce, nonce, sizeof chacha20i_ctx.nonce);
#if STMCPHR_IS_BIG
    chacha20i_ctx.nonce[0] = stmcphr_bswap32(chacha20i_ctx.nonce[0]);
    chacha20i_ctx.nonce[1] = stmcphr_bswap32(chacha20i_ctx.nonce[1]);
    chacha20i_ctx.nonce[2] = stmcphr_bswap32(chacha20i_ctx.nonce[2]);
#endif

    return 0;
}

int chacha20_setup_block(size_t number) {
    if ((number >> 32) > 0) return 1;
    chacha20i_ctx.block = number;
    return 0;
}

#define chacha20i_quarterround(M, a, b, c, d) do { \
    M[a] += M[b]; M[d] ^= M[a]; M[d] = chacha20i_rotl(M[d], 16); \
    M[c] += M[d]; M[b] ^= M[c]; M[b] = chacha20i_rotl(M[b], 12); \
    M[a] += M[b]; M[d] ^= M[a]; M[d] = chacha20i_rotl(M[d],  8); \
    M[c] += M[d]; M[b] ^= M[c]; M[b] = chacha20i_rotl(M[b],  7); \
} while (0)

void chacha20_begin_gen(void) {
    chacha20_word_t M[16]; int i;

    M[0] = 0x61707865; M[1] = 0x3320646e;
    M[2] = 0x79622d32; M[3] = 0x6b206574;
    M[12] = chacha20i_ctx.block;

    memcpy(M +  4, chacha20i_ctx.key  , sizeof chacha20i_ctx.key  );
    memcpy(M + 13, chacha20i_ctx.nonce, sizeof chacha20i_ctx.nonce);

    for (i = 0; i < 10; i++) {
        chacha20i_quarterround(M, 0, 4,  8, 12);
        chacha20i_quarterround(M, 1, 5,  9, 13);
        chacha20i_quarterround(M, 2, 6, 10, 14);
        chacha20i_quarterround(M, 3, 7, 11, 15);
        chacha20i_quarterround(M, 0, 5, 10, 15);
        chacha20i_quarterround(M, 1, 6, 11, 12);
        chacha20i_quarterround(M, 2, 7,  8, 13);
        chacha20i_quarterround(M, 3, 4,  9, 14);
    }

    M[0] += 0x61707865; M[1] += 0x3320646e;
    M[2] += 0x79622d32; M[3] += 0x6b206574;
    M[12] += chacha20i_ctx.block;

    for (i = 0; i < 8; i++) M[i +  4] += chacha20i_ctx.key[i];
    for (i = 0; i < 3; i++) M[i + 13] += chacha20i_ctx.nonce[i];

    chacha20i_ctx.remained = sizeof chacha20i_ctx.gamma;
    for (i = 0; i < 16; i++) {
#if STMCPHR_IS_BIG
        M[i] = stmcphr_bswap32(M[i]);
#endif
        memcpy(chacha20i_ctx.gamma + 4 * i, M + i, 4);
    }
}

void chacha20_take_gamma(void* dest, size_t count) {
    chacha20_byte_t* dst = dest; size_t min;
    while (count > 0) {
        min = count < chacha20i_ctx.remained
            ? count : chacha20i_ctx.remained;
        memcpy(dst, chacha20i_ctx.gamma +
            sizeof chacha20i_ctx.gamma - chacha20i_ctx.remained, min);
        dst += min; count -= min; chacha20i_ctx.remained -= min;

        if (chacha20i_ctx.remained == 0) {
            chacha20i_ctx.block += 1;
            chacha20_begin_gen();
        }
    }
}

void chacha20_skip_gamma(size_t count) {
    size_t min;
    while (count > 0) {
        min = count < chacha20i_ctx.remained
            ? count : chacha20i_ctx.remained;
        chacha20i_ctx.remained -= min;
        count -= min;

        if (chacha20i_ctx.remained == 0) {
            chacha20i_ctx.block += 1;
            chacha20_begin_gen();
        }
    }
}