#define STMCPHR_USE_ROTR32 hc256i_rotr

#include "hc256.h"
#include <string.h>
#include "config.h"

typedef stmcphr_u8_t  hc256_byte_t;
typedef stmcphr_u32_t hc256_word_t;

static struct {
    hc256_byte_t gamma[8192];
    hc256_word_t P[1024];
    hc256_word_t Q[1024];
    hc256_word_t key[8];
    hc256_word_t nonce[8];
    hc256_word_t remained;
} hc256i_ctx;

int hc256_setup_key(const void* key, int bits) {
    if (bits != 256) return 1;

    memcpy(hc256i_ctx.key, key, sizeof hc256i_ctx.key);
#if STMCPHR_IS_BIG
    hc256i_ctx.key[0] = stmcphr_bswap32(hc256i_ctx.key[0]);
    hc256i_ctx.key[1] = stmcphr_bswap32(hc256i_ctx.key[1]);
    hc256i_ctx.key[2] = stmcphr_bswap32(hc256i_ctx.key[2]);
    hc256i_ctx.key[3] = stmcphr_bswap32(hc256i_ctx.key[3]);
    hc256i_ctx.key[4] = stmcphr_bswap32(hc256i_ctx.key[4]);
    hc256i_ctx.key[5] = stmcphr_bswap32(hc256i_ctx.key[5]);
    hc256i_ctx.key[6] = stmcphr_bswap32(hc256i_ctx.key[6]);
    hc256i_ctx.key[7] = stmcphr_bswap32(hc256i_ctx.key[7]);
#endif

    return 0;
}

int hc256_setup_nonce(const void* nonce, int bits) {
    if (bits != 256) return 1;

    memcpy(hc256i_ctx.nonce, nonce, sizeof hc256i_ctx.nonce);
#if STMCPHR_IS_BIG
    hc256i_ctx.nonce[0] = stmcphr_bswap32(hc256i_ctx.nonce[0]);
    hc256i_ctx.nonce[1] = stmcphr_bswap32(hc256i_ctx.nonce[1]);
    hc256i_ctx.nonce[2] = stmcphr_bswap32(hc256i_ctx.nonce[2]);
    hc256i_ctx.nonce[3] = stmcphr_bswap32(hc256i_ctx.nonce[3]);
    hc256i_ctx.nonce[4] = stmcphr_bswap32(hc256i_ctx.nonce[4]);
    hc256i_ctx.nonce[5] = stmcphr_bswap32(hc256i_ctx.nonce[5]);
    hc256i_ctx.nonce[6] = stmcphr_bswap32(hc256i_ctx.nonce[6]);
    hc256i_ctx.nonce[7] = stmcphr_bswap32(hc256i_ctx.nonce[7]);
#endif

    return 0;
}

static hc256_word_t hc256i_f1(hc256_word_t x) {
    return hc256i_rotr(x, 7) ^ hc256i_rotr(x, 18) ^ (x >> 3);
}

static hc256_word_t hc256i_f2(hc256_word_t x) {
    return hc256i_rotr(x, 17) ^ hc256i_rotr(x, 19) ^ (x >> 10);
}

static hc256_word_t hc256i_g1(hc256_word_t x, hc256_word_t y) {
    return (hc256i_rotr(x, 10) ^ hc256i_rotr(y, 23)) + hc256i_ctx.Q[(x ^ y) & 1023];
}

static hc256_word_t hc256i_g2(hc256_word_t x, hc256_word_t y) {
    return (hc256i_rotr(x, 10) ^ hc256i_rotr(y, 23)) + hc256i_ctx.P[(x ^ y) & 1023];
}

static hc256_word_t hc256i_h1(hc256_word_t x) {
    hc256_byte_t x0, x1, x2, x3;
    x0 = (x >>  0) & 0xFF; x1 = (x >>  8) & 0xFF;
    x2 = (x >> 16) & 0xFF; x3 = (x >> 24) & 0xFF;
    return hc256i_ctx.Q[x0      ] + hc256i_ctx.Q[x1 + 256]
         + hc256i_ctx.Q[x2 + 512] + hc256i_ctx.Q[x3 + 768];
}

static hc256_word_t hc256i_h2(hc256_word_t x) {
    hc256_byte_t x0, x1, x2, x3;
    x0 = (x >>  0) & 0xFF; x1 = (x >>  8) & 0xFF;
    x2 = (x >> 16) & 0xFF; x3 = (x >> 24) & 0xFF;
    return hc256i_ctx.P[x0      ] + hc256i_ctx.P[x1 + 256]
         + hc256i_ctx.P[x2 + 512] + hc256i_ctx.P[x3 + 768];
}

static void hc256i_round(void) {
    hc256_word_t S, i;
    hc256i_ctx.remained = 8192;

    for (i = 0; i < 1024; i++) {
        hc256i_ctx.P[i] += hc256i_ctx.P[(i - 10) & 1023]
            + hc256i_g1(hc256i_ctx.P[(i - 3) & 1023], hc256i_ctx.P[(i + 1) & 1023]);
        S = hc256i_h1(hc256i_ctx.P[(i - 12) & 1023]) ^ hc256i_ctx.P[i];
#if STMCPHR_IS_BIG
        S = stmcphr_bswap32(S);
#endif
        memcpy(hc256i_ctx.gamma + 4 * i, &S, sizeof S);
    }
    for (i = 0; i < 1024; i++) {
        hc256i_ctx.Q[i] += hc256i_ctx.Q[(i - 10) & 1023]
            + hc256i_g2(hc256i_ctx.Q[(i - 3) & 1023], hc256i_ctx.Q[(i + 1) & 1023]);
        S = hc256i_h2(hc256i_ctx.Q[(i - 12) & 1023]) ^ hc256i_ctx.Q[i];
#if STMCPHR_IS_BIG
        S = stmcphr_bswap32(S);
#endif
        memcpy(hc256i_ctx.gamma + 4 * i + 4096, &S, sizeof S);
    }
}

void hc256_begin_gen(void) {
    hc256_word_t W[17], i;

    memcpy(W + 0, hc256i_ctx.key  , sizeof hc256i_ctx.key  );
    memcpy(W + 8, hc256i_ctx.nonce, sizeof hc256i_ctx.nonce);

    for (i = 16; i < 2560; i++) {
        W[16] = hc256i_f2(W[14]) + W[9] + hc256i_f1(W[1]) + W[0] + i;
        if ( 512 <= i && i < 1536) hc256i_ctx.P[i -  512] = W[16];
        if (1536 <= i && i < 2560) hc256i_ctx.Q[i - 1536] = W[16];
        memmove(W, W + 1, sizeof(hc256_word_t) * 16);
    }

    for (i = 0; i < 1024; i++)
        hc256i_ctx.P[i] += hc256i_ctx.P[(i - 10) & 1023]
            + hc256i_g1(hc256i_ctx.P[(i - 3) & 1023], hc256i_ctx.P[(i + 1) & 1023]);
    for (i = 0; i < 1024; i++)
        hc256i_ctx.Q[i] += hc256i_ctx.Q[(i - 10) & 1023]
            + hc256i_g2(hc256i_ctx.Q[(i - 3) & 1023], hc256i_ctx.Q[(i + 1) & 1023]);
    for (i = 0; i < 1024; i++)
        hc256i_ctx.P[i] += hc256i_ctx.P[(i - 10) & 1023]
            + hc256i_g1(hc256i_ctx.P[(i - 3) & 1023], hc256i_ctx.P[(i + 1) & 1023]);
    for (i = 0; i < 1024; i++)
        hc256i_ctx.Q[i] += hc256i_ctx.Q[(i - 10) & 1023]
            + hc256i_g2(hc256i_ctx.Q[(i - 3) & 1023], hc256i_ctx.Q[(i + 1) & 1023]);

    hc256i_round();
}

void hc256_take_gamma(void* dest, size_t count) {
    hc256_byte_t* dst = dest; size_t min;
    while (count > 0) {
        min = count < hc256i_ctx.remained
            ? count : hc256i_ctx.remained;
        memcpy(dst, hc256i_ctx.gamma + 8192 - hc256i_ctx.remained, min);
        dst += min; count -= min; hc256i_ctx.remained -= min;

        if (hc256i_ctx.remained == 0)
            hc256i_round();
    }
}

void hc256_skip_gamma(size_t count) {
    size_t min;
    while (count > 0) {
        min = count < hc256i_ctx.remained
            ? count : hc256i_ctx.remained;
        hc256i_ctx.remained -= min;
        count -= min;

        if (hc256i_ctx.remained == 0)
            hc256i_round();
    }
}