# Stream ciphers

> [!Warning]
> This code is not intended to provide true protection against real-world attacks. Instead, it serves as a demonstration of the algorithms.

Implementation of several stream ciphers.

## Documentation

All functions that return a value return `0` upon success, nonzero value otherwise.

### Ciphers

**`int <name>_setup_key(const void* key, int key_bits)`**  
Sets the key for later use.

**`int <name>_setup_nonce(const void* nonce, int nonce_bits)`**  
Sets the nonce for later use.

*[Actual for ChaCha20 and Salsa20]*  
**`int <name>_setup_block(size_t number)`**  
Sets the block number for later use.

**`void <name>_begin_gen(void)`**  
Initializes the internal state for gamma generation.

**`void <name>_take_gamma(void* dest, size_t count)`**  
Generates a gamma in the amount of `count` and saves it by the pointer `dest`.

**`void <name>_skip_gamma(size_t count)`**  
Generates a gamma in the amount of `count` and skips it.