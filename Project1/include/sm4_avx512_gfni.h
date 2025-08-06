#ifndef SM4_AVX512_GFNI_H
#define SM4_AVX512_GFNI_H

#include <stdint.h>

// AVX-512+GFNI检测函数
int sm4_avx512_gfni_available(void);

// AVX-512+GFNI优化SM4函数声明
void sm4_encrypt_avx512_gfni(const uint8_t *key, const uint8_t *plaintext, 
                              uint8_t *ciphertext, int num_blocks);
void sm4_decrypt_avx512_gfni(const uint8_t *key, const uint8_t *ciphertext, 
                              uint8_t *plaintext, int num_blocks);

#endif // SM4_AVX512_GFNI_H 