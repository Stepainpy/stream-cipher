/* Rabbit
 *
 * Length of key   - 128 bit
 * Length of nonce - 0 or 64 bit
 * Sources:
 *   https://datatracker.ietf.org/doc/html/rfc4503
 */

#ifndef RABBIT_STREAM_CIPHER_H
#define RABBIT_STREAM_CIPHER_H

#include <stddef.h>

int rabbit_setup_key  (const void* key  , int   key_bits);
int rabbit_setup_nonce(const void* nonce, int nonce_bits);

void rabbit_begin_gen(void);

void rabbit_take_gamma(void* dest, size_t count);
void rabbit_skip_gamma(            size_t count);

#endif /* RABBIT_STREAM_CIPHER_H */