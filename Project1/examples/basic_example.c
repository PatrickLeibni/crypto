#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sm4_basic.h"
#include "../include/sm4_ttable.h"
#include "../include/sm4_aesni.h"
#include "../include/sm4_gfni.h"
#include "../include/sm4_vprold.h"

// 测试密钥和明文
static const uint8_t test_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

static const uint8_t test_plaintext[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

// 打印数据块
void print_block(const char *label, const uint8_t *block) {
    printf("%s: ", label);
    for (int i = 0; i < 16; i++) {
        printf("%02x", block[i]);
    }
    printf("\n");
}

// 测试基本加密解密
void test_basic_encryption() {
    printf("=== 基本加密解密测试 ===\n");
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 基本实现
    sm4_encrypt_basic(test_key, test_plaintext, ciphertext);
    print_block("密文", ciphertext);
    
    sm4_decrypt_basic(test_key, ciphertext, decrypted);
    print_block("解密结果", decrypted);
    
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ 基本实现测试通过\n");
    } else {
        printf("✗ 基本实现测试失败\n");
    }
    printf("\n");
}

// 测试T-table优化
void test_ttable_optimization() {
    printf("=== T-table优化测试 ===\n");
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // T-table实现
    sm4_encrypt_ttable(test_key, test_plaintext, ciphertext);
    print_block("T-table密文", ciphertext);
    
    sm4_decrypt_ttable(test_key, ciphertext, decrypted);
    print_block("T-table解密结果", decrypted);
    
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ T-table优化测试通过\n");
    } else {
        printf("✗ T-table优化测试失败\n");
    }
    printf("\n");
}

// 测试AESNI优化
void test_aesni_optimization() {
    printf("=== AESNI优化测试 ===\n");
    
    if (!sm4_aesni_available()) {
        printf("⚠ CPU不支持AESNI指令集，跳过测试\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // AESNI实现
    sm4_encrypt_aesni(test_key, test_plaintext, ciphertext);
    print_block("AESNI密文", ciphertext);
    
    sm4_decrypt_aesni(test_key, ciphertext, decrypted);
    print_block("AESNI解密结果", decrypted);
    
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ AESNI优化测试通过\n");
    } else {
        printf("✗ AESNI优化测试失败\n");
    }
    printf("\n");
}

// 测试GFNI优化
void test_gfni_optimization() {
    printf("=== GFNI优化测试 ===\n");
    
    if (!sm4_gfni_available()) {
        printf("⚠ CPU不支持GFNI指令集，跳过测试\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // GFNI实现
    sm4_encrypt_gfni(test_key, test_plaintext, ciphertext);
    print_block("GFNI密文", ciphertext);
    
    sm4_decrypt_gfni(test_key, ciphertext, decrypted);
    print_block("GFNI解密结果", decrypted);
    
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ GFNI优化测试通过\n");
    } else {
        printf("✗ GFNI优化测试失败\n");
    }
    printf("\n");
}

// 测试VPROLD优化
void test_vprold_optimization() {
    printf("=== VPROLD优化测试 ===\n");
    
    if (!sm4_vprold_available()) {
        printf("⚠ CPU不支持VPROLD指令集，跳过测试\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // VPROLD实现
    sm4_encrypt_vprold(test_key, test_plaintext, ciphertext);
    print_block("VPROLD密文", ciphertext);
    
    sm4_decrypt_vprold(test_key, ciphertext, decrypted);
    print_block("VPROLD解密结果", decrypted);
    
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ VPROLD优化测试通过\n");
    } else {
        printf("✗ VPROLD优化测试失败\n");
    }
    printf("\n");
}

// 测试自动选择最优实现
void test_auto_optimization() {
    printf("=== 自动优化测试 ===\n");
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 使用基本实现作为默认
    sm4_encrypt_basic(test_key, test_plaintext, ciphertext);
    print_block("基本实现密文", ciphertext);
    
    sm4_decrypt_basic(test_key, ciphertext, decrypted);
    print_block("基本实现解密结果", decrypted);
    
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ 基本实现测试通过\n");
    } else {
        printf("✗ 基本实现测试失败\n");
    }
    printf("\n");
}

// 测试批量处理
void test_batch_processing() {
    printf("=== 批量处理测试 ===\n");
    
    const size_t blocks = 4;
    uint8_t data[16 * blocks];
    uint8_t encrypted[16 * blocks];
    uint8_t decrypted[16 * blocks];
    
    // 初始化测试数据
    for (size_t i = 0; i < blocks; i++) {
        memcpy(data + i * 16, test_plaintext, 16);
    }
    
    // 批量加密
    for (size_t i = 0; i < blocks; i++) {
        sm4_encrypt_basic(test_key, data + i * 16, encrypted + i * 16);
    }
    printf("批量加密完成，处理了 %zu 个数据块\n", blocks);
    
    // 批量解密
    for (size_t i = 0; i < blocks; i++) {
        sm4_decrypt_basic(test_key, encrypted + i * 16, decrypted + i * 16);
    }
    printf("批量解密完成\n");
    
    // 验证结果
    int all_correct = 1;
    for (size_t i = 0; i < blocks; i++) {
        if (memcmp(data + i * 16, decrypted + i * 16, 16) != 0) {
            all_correct = 0;
            break;
        }
    }
    
    if (all_correct) {
        printf("✓ 批量处理测试通过\n");
    } else {
        printf("✗ 批量处理测试失败\n");
    }
    printf("\n");
}

int main() {
    printf("SM4基本使用示例\n");
    printf("================\n\n");
    
    // 打印测试数据
    print_block("测试密钥", test_key);
    print_block("测试明文", test_plaintext);
    printf("\n");
    
    // 运行各种测试
    test_basic_encryption();
    test_ttable_optimization();
    test_aesni_optimization();
    test_gfni_optimization();
    test_vprold_optimization();
    test_auto_optimization();
    test_batch_processing();
    
    printf("所有测试完成！\n");
    return 0;
} 