#include "rc4.h"
#include <string.h>
#include "config.h"

typedef stmcphr_u8_t rc4_byte_t;

static struct {
    rc4_byte_t S[256];
    rc4_byte_t K[256];
    rc4_byte_t i, j;
    rc4_byte_t lenK; /* len(K) - 5 */
} rc4i_ctx;

#define rc4i_swap(a, b) do { rc4_byte_t t = (a); (a) = (b); (b) = t; } while (0)

int rc4_setup_key(const void* key, int bits) {
    if (bits < 40 || bits > 2048) return 1;
    if (bits % 8 != 0) return 1;
    memcpy(rc4i_ctx.K, key, bits / 8);
    rc4i_ctx.lenK = bits / 8 - 5;
    return 0;
}

int rc4_setup_nonce(const void* nonce, int bits) {
    return 0; (void)nonce; (void)bits; /* no cipher is required for this */
}

void rc4_begin_gen(void) {
    int i, j;
    rc4i_ctx.i = rc4i_ctx.j = 0;
    for (i     = 0; i < 256; i++) rc4i_ctx.S[i] = i;
    for (i = j = 0; i < 256; i++) {
        j = (j + rc4i_ctx.S[i] + rc4i_ctx.K[i % (rc4i_ctx.lenK + 5)]) & 255;
        rc4i_swap(rc4i_ctx.S[i], rc4i_ctx.S[j]);
    }
}

void rc4_take_gamma(void* dest, size_t count) {
    rc4_byte_t* dst = dest;
    while (count --> 0) {
        rc4i_ctx.j += rc4i_ctx.S[++rc4i_ctx.i];
        rc4i_swap(rc4i_ctx.S[rc4i_ctx.i], rc4i_ctx.S[rc4i_ctx.j]);
        *dst++ = rc4i_ctx.S[(
            rc4i_ctx.S[rc4i_ctx.i] + rc4i_ctx.S[rc4i_ctx.j]
        ) & 255];
    }
}

void rc4_skip_gamma(size_t count) {
    while (count --> 0) {
        rc4i_ctx.j += rc4i_ctx.S[++rc4i_ctx.i];
        rc4i_swap(rc4i_ctx.S[rc4i_ctx.i], rc4i_ctx.S[rc4i_ctx.j]);
    }
}