# SM4 API参考文档

## 概述

本文档提供SM4加密算法实现的核心API接口说明。

## 核心API

### 基本加密函数

```c
void sm4_encrypt_block(uint8_t *key, uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_block(uint8_t *key, uint8_t *ciphertext, uint8_t *plaintext);
```
**功能**: 加密/解密单个128位数据块

### 批量处理函数

```c
void sm4_encrypt_blocks(uint8_t *key, uint8_t *data, uint8_t *out, size_t blocks);
void sm4_decrypt_blocks(uint8_t *key, uint8_t *data, uint8_t *out, size_t blocks);
```
**功能**: 批量加密/解密多个数据块

## 优化版本API

| 实现 | 函数名 | 性能提升 | 要求 |
|------|--------|---------|------|
| T-table | `sm4_*_block_ttable` | 2-3x | 无 |
| AESNI | `sm4_*_block_aesni` | 3-5x | Intel AESNI |
| GFNI | `sm4_*_block_gfni` | 5-8x | GFNI指令 |
| VPROLD | `sm4_*_block_vprold` | 8-12x | VPROLD指令 |
| AVX-512+GFNI | `sm4_encrypt_blocks_avx512_gfni` | 10-20x | AVX-512+GFNI |
| AVX-512+VPROLD | `sm4_encrypt_blocks_avx512_vprold` | 10-20x | AVX-512+VPROLD |

## GCM模式API

```c
int sm4_gcm_encrypt(uint8_t *key, uint8_t *iv, size_t iv_len,
                    uint8_t *aad, size_t aad_len,
                    uint8_t *plaintext, size_t plaintext_len,
                    uint8_t *ciphertext, uint8_t *tag);

int sm4_gcm_decrypt(uint8_t *key, uint8_t *iv, size_t iv_len,
                    uint8_t *aad, size_t aad_len,
                    uint8_t *ciphertext, size_t ciphertext_len,
                    uint8_t *plaintext, uint8_t *tag);
```
**功能**: SM4-GCM模式加密/解密
**返回值**: 0表示成功，-1表示失败

## 自动选择API

```c
void sm4_encrypt_auto(uint8_t *key, uint8_t *plaintext, uint8_t *ciphertext);
void sm4_decrypt_auto(uint8_t *key, uint8_t *ciphertext, uint8_t *plaintext);
const char* sm4_get_best_implementation(void);
```
**功能**: 自动选择最优实现

## CPU特性检测

```c
int cpu_supports_aesni(void);
int cpu_supports_gfni(void);
int cpu_supports_vprold(void);
int cpu_supports_avx512(void);
```
**返回值**: 1表示支持，0表示不支持

## 工具函数

```c
void sm4_print_block(const char *label, uint8_t *block);
int sm4_hex_to_bytes(const char *hex, uint8_t *bytes, size_t len);
void sm4_bytes_to_hex(uint8_t *bytes, size_t len, char *hex);
const char* sm4_error_string(int error_code);
```

## 使用示例

### 基本使用
```c
#include "sm4_basic.h"

uint8_t key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                   0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
uint8_t plaintext[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                         0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
uint8_t ciphertext[16];

// 使用自动选择的最优实现
sm4_encrypt_auto(key, plaintext, ciphertext);
printf("最优实现: %s\n", sm4_get_best_implementation());
```

### GCM模式使用
```c
#include "sm4_gcm.h"

uint8_t key[16], iv[12], aad[16], plaintext[64], ciphertext[64], tag[16];

int result = sm4_gcm_encrypt(key, iv, 12, aad, 16, 
                            plaintext, 64, ciphertext, tag);
if (result == 0) {
    printf("GCM加密成功\n");
} else {
    printf("GCM加密失败: %s\n", sm4_error_string(result));
}
```

## 注意事项

1. **内存对齐**: SIMD优化版本要求数据按32字节对齐
2. **线程安全**: 所有函数都是线程安全的
3. **密钥管理**: 请安全存储和传输密钥
4. **错误检查**: 建议检查所有函数的返回值
5. **性能测试**: 在目标硬件上测试性能以选择最佳实现 