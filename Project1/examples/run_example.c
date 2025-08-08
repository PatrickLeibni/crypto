#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/sm4_basic.h"
#include "../include/sm4_ttable.h"
#include "../include/sm4_aesni.h"
#include "../include/sm4_gfni.h"
#include "../include/sm4_vprold.h"
#include "../include/sm4_gcm.h"

// 测试密钥
static const uint8_t test_key[16] = {
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

// 打印文本数据
void print_text(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%c", data[i]);
    }
    printf("\n");
}

// 基本加密解密示例
void basic_encryption_example() {
    printf("=== 基本加密解密示例 ===\n");
    
    const char *message = "Hello, SM4!";
    size_t message_len = strlen(message);
    
    // 确保数据长度是16字节的倍数
    size_t padded_len = ((message_len + 15) / 16) * 16;
    uint8_t *padded_data = malloc(padded_len);
    memset(padded_data, 0, padded_len);
    memcpy(padded_data, message, message_len);
    
    uint8_t *encrypted = malloc(padded_len);
    uint8_t *decrypted = malloc(padded_len);
    
    printf("原始消息: %s\n", message);
    print_hex("密钥", test_key, 16);
    printf("消息长度: %zu 字节\n\n", message_len);
    
    // 加密
    for (size_t i = 0; i < padded_len; i += 16) {
        sm4_encrypt_basic(test_key, padded_data + i, encrypted + i);
    }
    
    printf("✓ 加密完成\n");
    print_hex("密文", encrypted, padded_len);
    printf("\n");
    
    // 解密
    for (size_t i = 0; i < padded_len; i += 16) {
        sm4_decrypt_basic(test_key, encrypted + i, decrypted + i);
    }
    
    printf("✓ 解密完成\n");
    print_hex("解密结果", decrypted, padded_len);
    print_text("解密文本", decrypted, message_len);
    
    // 验证结果
    if (memcmp(padded_data, decrypted, message_len) == 0) {
        printf("✓ 加密解密验证成功\n");
    } else {
        printf("✗ 加密解密验证失败\n");
    }
    
    free(padded_data);
    free(encrypted);
    free(decrypted);
    printf("\n");
}

// GCM模式示例
void gcm_mode_example() {
    printf("=== GCM模式示例 ===\n");
    
    const char *message = "这是一个GCM模式测试消息";
    size_t message_len = strlen(message);
    
    uint8_t iv[12];
    uint8_t aad[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint8_t tag[16];
    uint8_t *encrypted = malloc(message_len);
    uint8_t *decrypted = malloc(message_len);
    
    // 生成随机IV
    sm4_gcm_generate_iv(iv);
    
    printf("原始消息: %s\n", message);
    print_hex("密钥", test_key, 16);
    print_hex("IV", iv, 12);
    print_hex("AAD", aad, 16);
    printf("消息长度: %zu 字节\n\n", message_len);
    
    // GCM加密
    int result = sm4_gcm_encrypt(test_key, iv, 12,
                                 (const uint8_t*)message, message_len,
                                 aad, 16,
                                 encrypted, tag);
    
    if (result == 0) {
        printf("✓ GCM加密成功\n");
        print_hex("密文", encrypted, message_len);
        print_hex("认证标签", tag, 16);
        printf("\n");
        
        // GCM解密
        result = sm4_gcm_decrypt(test_key, iv, 12,
                                encrypted, message_len,
                                aad, 16,
                                tag, decrypted);
        
        if (result == 0) {
            printf("✓ GCM解密成功\n");
            print_text("解密结果", decrypted, message_len);
            printf("✓ GCM认证验证通过\n");
        } else {
            printf("✗ GCM解密失败\n");
        }
    } else {
        printf("✗ GCM加密失败\n");
    }
    
    free(encrypted);
    free(decrypted);
    printf("\n");
}

// 性能对比示例
void performance_comparison() {
    printf("=== 性能对比示例 ===\n");
    
    const int iterations = 100000;
    const char *test_data = "SM4性能测试数据块";
    size_t data_len = strlen(test_data);
    
    // 确保数据长度是16字节的倍数
    size_t padded_len = ((data_len + 15) / 16) * 16;
    uint8_t *padded_data = malloc(padded_len);
    uint8_t *encrypted = malloc(padded_len);
    uint8_t *decrypted = malloc(padded_len);
    
    memset(padded_data, 0, padded_len);
    memcpy(padded_data, test_data, data_len);
    
    printf("测试数据: %s\n", test_data);
    printf("迭代次数: %d\n", iterations);
    printf("数据长度: %zu 字节\n\n", padded_len);
    
    // 测试基本实现
    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        for (size_t j = 0; j < padded_len; j += 16) {
            sm4_encrypt_basic(test_key, padded_data + j, encrypted + j);
        }
    }
    clock_t end = clock();
    double basic_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("基本实现: %.6f 秒\n", basic_time);
    printf("吞吐量: %.2f MB/s\n", 
           (iterations * padded_len) / (basic_time * 1024 * 1024));
    
    // 测试T-table优化
    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (size_t j = 0; j < padded_len; j += 16) {
            sm4_encrypt_ttable(test_key, padded_data + j, encrypted + j);
        }
    }
    end = clock();
    double ttable_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("T-table优化: %.6f 秒\n", ttable_time);
    printf("吞吐量: %.2f MB/s\n", 
           (iterations * padded_len) / (ttable_time * 1024 * 1024));
    
    // 测试AESNI优化
    if (sm4_aesni_available()) {
        start = clock();
        for (int i = 0; i < iterations; i++) {
            for (size_t j = 0; j < padded_len; j += 16) {
                sm4_encrypt_aesni(test_key, padded_data + j, encrypted + j);
            }
        }
        end = clock();
        double aesni_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        printf("AESNI优化: %.6f 秒\n", aesni_time);
        printf("吞吐量: %.2f MB/s\n", 
               (iterations * padded_len) / (aesni_time * 1024 * 1024));
    } else {
        printf("AESNI优化: 不支持\n");
    }
    
    free(padded_data);
    free(encrypted);
    free(decrypted);
    printf("\n");
}

// 错误处理示例
void error_handling_example() {
    printf("=== 错误处理示例 ===\n");
    
    // 测试GCM错误处理
    printf("测试GCM错误处理:\n");
    uint8_t iv[12] = {0};
    uint8_t tag[16];
    uint8_t encrypted[16];
    
    int result = sm4_gcm_encrypt(NULL, iv, 12, test_key, 16, NULL, 0, encrypted, tag);
    printf("  空密钥: %s\n", result == -1 ? "✓ 正确处理" : "✗ 未处理");
    
    result = sm4_gcm_encrypt(test_key, NULL, 12, test_key, 16, NULL, 0, encrypted, tag);
    printf("  空IV: %s\n", result == -1 ? "✓ 正确处理" : "✗ 未处理");
    
    result = sm4_gcm_encrypt(test_key, iv, 10, test_key, 16, NULL, 0, encrypted, tag);
    printf("  错误IV长度: %s\n", result == -1 ? "✓ 正确处理" : "✗ 未处理");
       
    printf("\n");
}

int main() {
    printf("SM4运行示例\n");
    printf("==========\n\n");
    
    // 显示CPU特性
    printf("CPU特性支持:\n");
    printf("  AESNI: %s\n", sm4_aesni_available() ? "✓ 支持" : "✗ 不支持");
    printf("  GFNI: %s\n", sm4_gfni_available() ? "✓ 支持" : "✗ 不支持");
    printf("  VPROLD: %s\n", sm4_vprold_available() ? "✓ 支持" : "✗ 不支持");
    printf("\n");
    
    // 运行各种示例
    basic_encryption_example();
    gcm_mode_example();
    performance_comparison();
    error_handling_example();
    
    printf("运行示例完成！\n");
    return 0;
} 
