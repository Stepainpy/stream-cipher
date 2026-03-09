/* Salsa20
 *
 * Length of key   - 256 bit
 * Length of nonce - 64 bit
 * Sources:
 *   https://cr.yp.to/snuffle/spec.pdf
 *   https://en.wikipedia.org/wiki/Salsa20
 */

#ifndef SALSA20_STREAM_CIPHER_H
#define SALSA20_STREAM_CIPHER_H

#include <stddef.h>

int salsa20_setup_key  (const void* key  , int   key_bits);
int salsa20_setup_nonce(const void* nonce, int nonce_bits);
int salsa20_setup_block(size_t number);

void salsa20_begin_gen(void);

void salsa20_take_gamma(void* dest, size_t count);
void salsa20_skip_gamma(            size_t count);

#endif /* SALSA20_STREAM_CIPHER_H */