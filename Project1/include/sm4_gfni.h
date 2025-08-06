#ifndef SM4_GFNI_H
#define SM4_GFNI_H

#include <stdint.h>

// GFNI检测函数
int sm4_gfni_available(void);

// GFNI优化SM4函数声明
void sm4_encrypt_gfni(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_gfni(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

// 批量GFNI优化
void sm4_encrypt_gfni_batch(const uint8_t *key, const uint8_t *plaintext, 
                             uint8_t *ciphertext, int num_blocks);
void sm4_decrypt_gfni_batch(const uint8_t *key, const uint8_t *ciphertext, 
                             uint8_t *plaintext, int num_blocks);

#endif // SM4_GFNI_H 