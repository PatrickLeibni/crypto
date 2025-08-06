#ifndef SM4_BASIC_H
#define SM4_BASIC_H

#include <stdint.h>

// 基本SM4函数声明
void sm4_encrypt(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

#endif // SM4_BASIC_H 