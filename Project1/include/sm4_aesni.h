#ifndef SM4_AESNI_H
#define SM4_AESNI_H

#include <stdint.h>

// AESNI检测函数
int sm4_aesni_available(void);

// AESNI优化SM4函数声明
void sm4_encrypt_aesni(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_aesni(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

#endif // SM4_AESNI_H 