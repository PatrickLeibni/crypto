#ifndef SM4_TTABLE_H
#define SM4_TTABLE_H

#include <stdint.h>

// T-table优化SM4函数声明
void sm4_encrypt_ttable(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_ttable(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

#endif // SM4_TTABLE_H 