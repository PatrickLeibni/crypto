#ifndef SM4_GCM_H
#define SM4_GCM_H

#include <stdint.h>
#include <stddef.h>

// SM4-GCM 函数声明

// 生成IV
void sm4_gcm_generate_iv(uint8_t *iv);

// 基础版本加密
int sm4_gcm_encrypt(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                     const uint8_t *plaintext, size_t plaintext_len,
                     const uint8_t *aad, size_t aad_len,
                     uint8_t *ciphertext, uint8_t *tag);

// 基础版本解密
int sm4_gcm_decrypt(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                     const uint8_t *ciphertext, size_t ciphertext_len,
                     const uint8_t *aad, size_t aad_len,
                     const uint8_t *tag, uint8_t *plaintext);

// 优化版本加密
int sm4_gcm_encrypt_optimized(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                               const uint8_t *plaintext, size_t plaintext_len,
                               const uint8_t *aad, size_t aad_len,
                               uint8_t *ciphertext, uint8_t *tag);

// 优化版本解密
int sm4_gcm_decrypt_optimized(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                               const uint8_t *ciphertext, size_t ciphertext_len,
                               const uint8_t *aad, size_t aad_len,
                               const uint8_t *tag, uint8_t *plaintext);

#endif // SM4_GCM_H 