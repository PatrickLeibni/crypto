#ifndef SM4_VPROLD_H
#define SM4_VPROLD_H

#include <stdint.h>
#include <immintrin.h>

// ============================================================================
// 1. 基本SM4实现
// ============================================================================

// SM4 S盒
extern const uint8_t SM4_SBOX[256];

// 基本SM4加密/解密函数
void sm4_encrypt_basic(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_basic(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

// 兼容性函数
void sm4_encrypt(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

// ============================================================================
// 2. VPROLD优化实现
// ============================================================================

// VPROLD检测函数
int sm4_vprold_available(void);

// VPROLD优化的SM4实现
void sm4_encrypt_vprold(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_vprold(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

// 批量VPROLD优化
void sm4_encrypt_vprold_batch(const uint8_t *key, const uint8_t *plaintext, 
                               uint8_t *ciphertext, int num_blocks);
void sm4_decrypt_vprold_batch(const uint8_t *key, const uint8_t *ciphertext, 
                               uint8_t *plaintext, int num_blocks);

// ============================================================================
// 6. 混合优化实现
// ============================================================================

// 自动选择最佳实现的函数
void sm4_encrypt_auto(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_auto(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext);

// ============================================================================
// 3. 性能测试函数
// ============================================================================

// 性能测试函数
void sm4_performance_test(const uint8_t *key, int num_blocks);

// 获取当前最佳实现名称
const char* sm4_get_best_implementation(void);

#endif // SM4_VPROLD_H 