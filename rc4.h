/* Implementation of stream cipher RC4 */

#ifndef RC4_CIPHER_H
#define RC4_CIPHER_H

#include <stdint.h>

#ifndef RC4_DEF
#define RC4_DEF
#endif

typedef struct {
    uint8_t S[256];
    uint16_t i, j;
} rc4_state_t;

RC4_DEF void rc4_key_init(rc4_state_t* state, const void* key, size_t klen);
RC4_DEF void rc4_skip_gamma(rc4_state_t* state, size_t count);
RC4_DEF uint8_t rc4_create_gamma(rc4_state_t* state);

#endif // RC4_CIPHER_H

#ifdef RC4_IMPLEMENTATION

#include <stddef.h>

static inline void __rc4_swap(uint8_t* a, uint8_t* b) {
    uint8_t t = *a; *a = *b; *b = t;
}

void rc4_key_init(rc4_state_t* s, const void* key, size_t klen) {
    const uint8_t* K = key;

    s->i = s->j = 0;
    for (size_t i = 0; i < 256; i++)
        s->S[i] = i;
    
    size_t j = 0;
    for (size_t i = 0; i < 256; i++) {
        j = (j + s->S[i] + K[i % klen]) % 256;
        __rc4_swap(s->S + i, s->S + j);
    }
}

void rc4_skip_gamma(rc4_state_t* s, size_t count) {
    while (count --> 0) {
        s->i = (s->i + 1) % 256;
        s->j = (s->j + s->S[s->i]) % 256;
        __rc4_swap(s->S + s->i, s->S + s->j);
    }
}

uint8_t rc4_create_gamma(rc4_state_t* s) {
    s->i = (s->i + 1) % 256;
    s->j = (s->j + s->S[s->i]) % 256;
    __rc4_swap(s->S + s->i, s->S + s->j);
    uint16_t t = (
        (uint16_t)s->S[s->i] +
        (uint16_t)s->S[s->j]
    ) % 256;
    return s->S[t];
}

#endif // RC4_IMPLEMENTATION