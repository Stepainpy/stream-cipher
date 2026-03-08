/* Rivest's cipher 4 (RC4)
 *
 * Length of key   - from 40 to 2048 bit
 * Length of nonce - none
 * Sorces:
 *   https://en.wikipedia.org/wiki/RC4
 *   https://datatracker.ietf.org/doc/html/rfc6229
 */

#ifndef RC4_STREAM_CIPHER_H
#define RC4_STREAM_CIPHER_H

#include <stddef.h>

int rc4_setup_key(const void* key, int key_bits);

void rc4_begin_gen(void);

void rc4_take_gamma(void* dest, size_t count);
void rc4_skip_gamma(            size_t count);

#endif /* RC4_STREAM_CIPHER_H */