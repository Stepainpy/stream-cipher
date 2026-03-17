/* HC-256
 *
 * Length of key   - 256 bit
 * Length of nonce - 256 bit
 * Sources:
 *   https://cr.yp.to/streamciphers/hc-256/desc.pdf
 */

#ifndef HC256_STREAM_CIPHER_H
#define HC256_STREAM_CIPHER_H

#include <stddef.h>

int hc256_setup_key  (const void* key  , int   key_bits);
int hc256_setup_nonce(const void* nonce, int nonce_bits);

void hc256_begin_gen(void);

void hc256_take_gamma(void* dest, size_t count);
void hc256_skip_gamma(            size_t count);

#endif /* HC256_STREAM_CIPHER_H */