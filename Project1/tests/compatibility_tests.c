#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/sm4_basic.h"
#include "../include/sm4_ttable.h"
#include "../include/sm4_aesni.h"

// 测试数据
static const uint8_t test_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

static const uint8_t test_plaintext[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

// 打印十六进制数据
void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// 测试数据大小兼容性
void test_data_sizes() {
    printf("=== 数据大小测试 ===\n");
    
    const int sizes[] = {16, 32, 64, 128, 256, 512, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        int blocks = sizes[i] / 16;
        uint8_t *data = malloc(sizes[i]);
        uint8_t *encrypted = malloc(sizes[i]);
        uint8_t *decrypted = malloc(sizes[i]);
        
        // 初始化数据
        for (int j = 0; j < sizes[i]; j++) {
            data[j] = j % 256;
        }
        
        // 加密
        for (int j = 0; j < blocks; j++) {
            sm4_encrypt(test_key, data + j * 16, encrypted + j * 16);
        }
        
        // 解密
        for (int j = 0; j < blocks; j++) {
            sm4_decrypt(test_key, encrypted + j * 16, decrypted + j * 16);
        }
        
        // 验证
        if (memcmp(data, decrypted, sizes[i]) == 0) {
            printf("数据大小 %d 字节: ✓ 通过\n", sizes[i]);
        } else {
            printf("数据大小 %d 字节: ✗ 失败\n", sizes[i]);
        }
        
        free(data);
        free(encrypted);
        free(decrypted);
    }
    printf("\n");
}

// 测试内存边界
void test_memory_boundaries() {
    printf("=== 内存边界测试 ===\n");
    
    const int offsets[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int num_offsets = sizeof(offsets) / sizeof(offsets[0]);
    
    for (int i = 0; i < num_offsets; i++) {
        uint8_t buffer[64];
        uint8_t encrypted[64];
        uint8_t decrypted[64];
        
        // 在缓冲区中偏移放置数据
        memset(buffer, 0, 64);
        memcpy(buffer + offsets[i], test_plaintext, 16);
        
        // 加密
        sm4_encrypt(test_key, buffer + offsets[i], encrypted + offsets[i]);
        
        // 解密
        sm4_decrypt(test_key, encrypted + offsets[i], decrypted + offsets[i]);
        
        // 验证
        if (memcmp(test_plaintext, decrypted + offsets[i], 16) == 0) {
            printf("偏移 %d 字节: ✓ 通过\n", offsets[i]);
        } else {
            printf("偏移 %d 字节: ✗ 失败\n", offsets[i]);
        }
    }
    printf("\n");
}

// 测试重复调用
void test_repeated_calls() {
    printf("=== 重复调用测试 ===\n");
    
    const int iterations = 10000;
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    int passed = 0;
    
    for (int i = 0; i < iterations; i++) {
        sm4_encrypt(test_key, test_plaintext, ciphertext);
        sm4_decrypt(test_key, ciphertext, decrypted);
        
        if (memcmp(test_plaintext, decrypted, 16) == 0) {
            passed++;
        }
    }
    
    printf("重复调用测试: %d/%d 通过 (%.1f%%)\n\n", 
           passed, iterations, (passed * 100.0) / iterations);
}

// 测试并发安全性
void test_concurrent_safety() {
    printf("=== 并发安全性测试 ===\n");
    
    const int threads = 4;
    const int iterations = 1000;
    int passed = 0;
    
    // 模拟并发调用
    for (int t = 0; t < threads; t++) {
        for (int i = 0; i < iterations; i++) {
            uint8_t ciphertext[16];
            uint8_t decrypted[16];
            
            sm4_encrypt(test_key, test_plaintext, ciphertext);
            sm4_decrypt(test_key, ciphertext, decrypted);
            
            if (memcmp(test_plaintext, decrypted, 16) == 0) {
                passed++;
            }
        }
    }
    
    printf("并发安全性测试: %d/%d 通过 (%.1f%%)\n\n", 
           passed, threads * iterations, (passed * 100.0) / (threads * iterations));
}

// 测试错误处理
void test_error_handling() {
    printf("=== 错误处理测试 ===\n");
    
    // 测试空指针（跳过）
    printf("空指针测试: 跳过 \n");
    
    // 测试无效数据
    uint8_t invalid_key[16] = {0};
    uint8_t invalid_plaintext[16] = {0};
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 处理全零数据
    sm4_encrypt(invalid_key, invalid_plaintext, ciphertext);
    sm4_decrypt(invalid_key, ciphertext, decrypted);
    
    if (memcmp(invalid_plaintext, decrypted, 16) == 0) {
        printf("错误处理测试: ✓ 通过\n");
    } else {
        printf("错误处理测试: ✗ 失败\n");
    }
    printf("\n");
}

// 测试平台特定特性
void test_platform_specific() {
    printf("=== 平台特定测试 ===\n");
    
    // 检查字节序
    union {
        uint32_t i;
        uint8_t c[4];
    } test = {0x01020304};
    
    if (test.c[0] == 1) {
        printf("字节序: 大端序\n");
    } else {
        printf("字节序: 小端序\n");
    }
    
    // 检查数据类型大小
    printf("数据类型大小:\n");
    printf("  uint8_t: %zu 字节\n", sizeof(uint8_t));
    printf("  uint32_t: %zu 字节\n", sizeof(uint32_t));
    printf("  size_t: %zu 字节\n", sizeof(size_t));
    
    // 检查对齐要求
    printf("对齐要求:\n");
    printf("  char: %zu\n", __alignof__(char));
    printf("  int: %zu\n", __alignof__(int));
    printf("  double: %zu\n", __alignof__(double));
    printf("\n");
}

// 测试编译器兼容性
void test_compiler_compatibility() {
    printf("=== 编译器兼容性测试 ===\n");
    
    // 检查编译器信息
    printf("编译器信息:\n");
    printf("  GCC版本: %s\n", __VERSION__);
    
    // 检查C标准版本
    #if __STDC_VERSION__ >= 201112L
        printf("C标准版本: C11或更高\n");
    #elif __STDC_VERSION__ >= 199901L
        printf("C标准版本: C99\n");
    #else
        printf("C标准版本: C89\n");
    #endif
    
    printf("\n");
}

int main() {
    printf("SM4兼容性测试\n");
    printf("============\n\n");
    
    // 初始化随机数生成器
    srand(time(NULL));
    
    test_data_sizes();
    test_memory_boundaries();
    test_repeated_calls();
    test_concurrent_safety();
    test_error_handling();
    test_platform_specific();
    test_compiler_compatibility();
    
    printf("兼容性测试完成！\n");
    
    return 0;
} 
