#define STMCPHR_USE_ROTL32 rabbiti_rotl

#include "rabbit.h"
#include <string.h>
#include "config.h"

typedef stmcphr_u8_t  rabbit_byte_t;
typedef stmcphr_u16_t rabbit_half_t;
typedef stmcphr_u32_t rabbit_word_t;
typedef stmcphr_u64_t rabbit_text_t;

static const rabbit_word_t rabbiti_A[8];

static struct {
    rabbit_byte_t gamma[16];
    rabbit_word_t X[8], C[8];
    rabbit_half_t K[8], N[4];
    rabbit_byte_t remained;
    rabbit_byte_t carry, has_nonce;
} rabbiti_ctx;

static void rabbiti_round(void) {
    rabbit_word_t G[8], new; int i;

    for (i = 0; i < 8; i++) {
        new = rabbiti_ctx.C[i] + rabbiti_A[i] + rabbiti_ctx.carry;
        rabbiti_ctx.carry = new < rabbiti_ctx.C[i];
        rabbiti_ctx.C[i] = new;
    }

    for (i = 0; i < 8; i++) {
        rabbit_text_t uv = (rabbit_word_t)(rabbiti_ctx.X[i] + rabbiti_ctx.C[i]);
        uv *= uv; G[i] = (uv >> 32) ^ uv;
    }

    rabbiti_ctx.X[0] = G[0] + rabbiti_rotl(G[7], 16) + rabbiti_rotl(G[6], 16);
    rabbiti_ctx.X[1] = G[1] + rabbiti_rotl(G[0],  8) +              G[7]     ;
    rabbiti_ctx.X[2] = G[2] + rabbiti_rotl(G[1], 16) + rabbiti_rotl(G[0], 16);
    rabbiti_ctx.X[3] = G[3] + rabbiti_rotl(G[2],  8) +              G[1]     ;
    rabbiti_ctx.X[4] = G[4] + rabbiti_rotl(G[3], 16) + rabbiti_rotl(G[2], 16);
    rabbiti_ctx.X[5] = G[5] + rabbiti_rotl(G[4],  8) +              G[3]     ;
    rabbiti_ctx.X[6] = G[6] + rabbiti_rotl(G[5], 16) + rabbiti_rotl(G[4], 16);
    rabbiti_ctx.X[7] = G[7] + rabbiti_rotl(G[6],  8) +              G[5]     ;
}

static void rabbiti_load_gamma(void) {
    rabbit_half_t S;
    rabbiti_ctx.remained = sizeof rabbiti_ctx.gamma;

    S = (rabbiti_ctx.X[6] >> 16) ^ rabbiti_ctx.X[1];
    rabbiti_ctx.gamma[ 0] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[ 1] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[3] >> 16) ^ rabbiti_ctx.X[6];
    rabbiti_ctx.gamma[ 2] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[ 3] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[4] >> 16) ^ rabbiti_ctx.X[7];
    rabbiti_ctx.gamma[ 4] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[ 5] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[1] >> 16) ^ rabbiti_ctx.X[4];
    rabbiti_ctx.gamma[ 6] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[ 7] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[2] >> 16) ^ rabbiti_ctx.X[5];
    rabbiti_ctx.gamma[ 8] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[ 9] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[7] >> 16) ^ rabbiti_ctx.X[2];
    rabbiti_ctx.gamma[10] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[11] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[0] >> 16) ^ rabbiti_ctx.X[3];
    rabbiti_ctx.gamma[12] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[13] = (S >> 0) & 0xFF;
    S = (rabbiti_ctx.X[5] >> 16) ^ rabbiti_ctx.X[0];
    rabbiti_ctx.gamma[14] = (S >> 8) & 0xFF;
    rabbiti_ctx.gamma[15] = (S >> 0) & 0xFF;
}

int rabbit_setup_key(const void* key, int bits) {
    const rabbit_byte_t* K = key; int i;
    if (bits != 128) return 1;

    for (i = 0; i < 8; i++)
        rabbiti_ctx.K[7 - i] = (rabbit_half_t)K[2 * i] << 8 | K[2 * i + 1];

    return 0;
}

int rabbit_setup_nonce(const void* nonce, int bits) {
    const rabbit_byte_t* N = nonce; int i;

    if (bits == 0) {
        memset(rabbiti_ctx.N, 0, sizeof rabbiti_ctx.N);
        rabbiti_ctx.has_nonce = 0;
        return 0;
    } else if (bits != 64)
        return 1;

    for (i = 0; i < 4; i++)
        rabbiti_ctx.N[3 - i] = (rabbit_half_t)N[2 * i] << 8 | N[2 * i + 1];
    rabbiti_ctx.has_nonce = 1;

    return 0;
}

void rabbit_begin_gen(void) {
    int i;

    rabbiti_ctx.carry = 0;

    for (i = 0; i < 8; i++)
        if (i % 2 == 0) {
            rabbiti_ctx.X[i] = (rabbit_word_t)rabbiti_ctx.K[(i + 1) & 7] << 16 | rabbiti_ctx.K[(i + 0) & 7];
            rabbiti_ctx.C[i] = (rabbit_word_t)rabbiti_ctx.K[(i + 4) & 7] << 16 | rabbiti_ctx.K[(i + 5) & 7];
        } else {
            rabbiti_ctx.X[i] = (rabbit_word_t)rabbiti_ctx.K[(i + 5) & 7] << 16 | rabbiti_ctx.K[(i + 4) & 7];
            rabbiti_ctx.C[i] = (rabbit_word_t)rabbiti_ctx.K[(i + 0) & 7] << 16 | rabbiti_ctx.K[(i + 1) & 7];
        }

    rabbiti_round(); rabbiti_round();
    rabbiti_round(); rabbiti_round();

    for (i = 0; i < 8; i++)
        rabbiti_ctx.C[i] ^= rabbiti_ctx.X[(i + 4) & 7];

    if (rabbiti_ctx.has_nonce) {
        rabbiti_ctx.C[0] ^= (rabbit_word_t)rabbiti_ctx.N[1] << 16 | rabbiti_ctx.N[0];
        rabbiti_ctx.C[1] ^= (rabbit_word_t)rabbiti_ctx.N[3] << 16 | rabbiti_ctx.N[1];
        rabbiti_ctx.C[2] ^= (rabbit_word_t)rabbiti_ctx.N[3] << 16 | rabbiti_ctx.N[2];
        rabbiti_ctx.C[3] ^= (rabbit_word_t)rabbiti_ctx.N[2] << 16 | rabbiti_ctx.N[0];
        rabbiti_ctx.C[4] ^= (rabbit_word_t)rabbiti_ctx.N[1] << 16 | rabbiti_ctx.N[0];
        rabbiti_ctx.C[5] ^= (rabbit_word_t)rabbiti_ctx.N[3] << 16 | rabbiti_ctx.N[1];
        rabbiti_ctx.C[6] ^= (rabbit_word_t)rabbiti_ctx.N[3] << 16 | rabbiti_ctx.N[2];
        rabbiti_ctx.C[7] ^= (rabbit_word_t)rabbiti_ctx.N[2] << 16 | rabbiti_ctx.N[0];

        rabbiti_round(); rabbiti_round();
        rabbiti_round(); rabbiti_round();
    }

    rabbiti_round();
    rabbiti_load_gamma();
}

void rabbit_take_gamma(void* dest, size_t count) {
    rabbit_byte_t* dst = dest; size_t min;
    while (count > 0) {
        min = count < rabbiti_ctx.remained
            ? count : rabbiti_ctx.remained;
        memcpy(dst, rabbiti_ctx.gamma +
            sizeof rabbiti_ctx.gamma - rabbiti_ctx.remained, min);
        dst += min; count -= min; rabbiti_ctx.remained -= min;

        if (rabbiti_ctx.remained == 0) {
            rabbiti_round();
            rabbiti_load_gamma();
        }
    }
}

void rabbit_skip_gamma(size_t count) {
    size_t min;
    while (count > 0) {
        min = count < rabbiti_ctx.remained
            ? count : rabbiti_ctx.remained;
        rabbiti_ctx.remained -= min;
        count -= min;

        if (rabbiti_ctx.remained == 0) {
            rabbiti_round();
            rabbiti_load_gamma();
        }
    }
}

static const rabbit_word_t rabbiti_A[8] = {
    0x4d34d34d, 0xd34d34d3, 0x34d34d34, 0x4d34d34d,
    0xd34d34d3, 0x34d34d34, 0x4d34d34d, 0xd34d34d3
};