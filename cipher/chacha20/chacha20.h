/* ChaCha20
 *
 * Length of key   - 256 bit
 * Length of nonce - 96 bit
 * Sources:
 *   https://datatracker.ietf.org/doc/html/rfc8439
 */

#ifndef CHACHA20_STREAM_CIPHER_H
#define CHACHA20_STREAM_CIPHER_H

#include <stddef.h>

int chacha20_setup_key  (const void* key  , int   key_bits);
int chacha20_setup_nonce(const void* nonce, int nonce_bits);
int chacha20_setup_block(size_t number);

void chacha20_begin_gen(void);

void chacha20_take_gamma(void* dest, size_t count);
void chacha20_skip_gamma(            size_t count);

#endif /* CHACHA20_STREAM_CIPHER_H */