#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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

// 性能测试结构
typedef struct {
    const char *name;
    void (*encrypt_func)(const uint8_t*, const uint8_t*, uint8_t*);
    void (*decrypt_func)(const uint8_t*, const uint8_t*, uint8_t*);
    int available;
    double encrypt_time;
    double decrypt_time;
} optimization_test_t;

// 性能测试函数
void benchmark_optimization(const char *name, 
                          void (*encrypt_func)(const uint8_t*, const uint8_t*, uint8_t*),
                          void (*decrypt_func)(const uint8_t*, const uint8_t*, uint8_t*),
                          int available) {
    if (!available) {
        printf("⚠ %s: 不支持，跳过测试\n", name);
        return;
    }
    
    const int iterations = 100000;
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 预热
    for (int i = 0; i < 1000; i++) {
        encrypt_func(test_key, test_plaintext, ciphertext);
        decrypt_func(test_key, ciphertext, decrypted);
    }
    
    // 加密性能测试
    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        encrypt_func(test_key, test_plaintext, ciphertext);
    }
    clock_t end = clock();
    double encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // 解密性能测试
    start = clock();
    for (int i = 0; i < iterations; i++) {
        decrypt_func(test_key, ciphertext, decrypted);
    }
    end = clock();
    double decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // 验证正确性
    int correct = (memcmp(test_plaintext, decrypted, 16) == 0);
    
    printf("✓ %s:\n", name);
    printf("  加密: %.6f秒 (%d次), %.2f MB/s\n", 
           encrypt_time, iterations, 
           (iterations * 16.0) / (encrypt_time * 1024 * 1024));
    printf("  解密: %.6f秒 (%d次), %.2f MB/s\n", 
           decrypt_time, iterations,
           (iterations * 16.0) / (decrypt_time * 1024 * 1024));
    printf("  正确性: %s\n\n", correct ? "✓ 通过" : "✗ 失败");
}

// 批量性能测试
void benchmark_batch_processing() {
    printf("=== 批量处理性能测试 ===\n");
    
    const size_t blocks = 1000;
    const int iterations = 100;
    uint8_t *data = malloc(16 * blocks);
    uint8_t *encrypted = malloc(16 * blocks);
    uint8_t *decrypted = malloc(16 * blocks);
    
    // 初始化测试数据
    for (size_t i = 0; i < blocks; i++) {
        memcpy(data + i * 16, test_plaintext, 16);
    }
    
    // 测试基本实现的批量处理
    clock_t start = clock();
    for (int iter = 0; iter < iterations; iter++) {
        for (size_t i = 0; i < blocks; i++) {
            sm4_encrypt_basic(test_key, data + i * 16, encrypted + i * 16);
        }
    }
    clock_t end = clock();
    double encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    start = clock();
    for (int iter = 0; iter < iterations; iter++) {
        for (size_t i = 0; i < blocks; i++) {
            sm4_decrypt_basic(test_key, encrypted + i * 16, decrypted + i * 16);
        }
    }
    end = clock();
    double decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("批量处理 (%zu 块, %d 次迭代):\n", blocks, iterations);
    printf("  加密: %.6f秒, %.2f MB/s\n", 
           encrypt_time, (blocks * iterations * 16.0) / (encrypt_time * 1024 * 1024));
    printf("  解密: %.6f秒, %.2f MB/s\n", 
           decrypt_time, (blocks * iterations * 16.0) / (decrypt_time * 1024 * 1024));
    
    // 验证批量处理正确性
    int all_correct = 1;
    for (size_t i = 0; i < blocks; i++) {
        if (memcmp(data + i * 16, decrypted + i * 16, 16) != 0) {
            all_correct = 0;
            break;
        }
    }
    printf("  正确性: %s\n\n", all_correct ? "✓ 通过" : "✗ 失败");
    
    free(data);
    free(encrypted);
    free(decrypted);
}

// 内存使用测试
void benchmark_memory_usage() {
    printf("=== 内存使用测试 ===\n");
    
    const size_t data_size = 1024 * 1024; // 1MB
    uint8_t *large_data = malloc(data_size);
    uint8_t *encrypted = malloc(data_size);
    uint8_t *decrypted = malloc(data_size);
    
    // 初始化大数据
    for (size_t i = 0; i < data_size; i++) {
        large_data[i] = (uint8_t)(i & 0xFF);
    }
    
    const int blocks = data_size / 16;
    clock_t start = clock();
    for (size_t i = 0; i < blocks; i++) {
        sm4_encrypt_basic(test_key, large_data + i * 16, encrypted + i * 16);
    }
    clock_t end = clock();
    double encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    start = clock();
    for (size_t i = 0; i < blocks; i++) {
        sm4_decrypt_basic(test_key, encrypted + i * 16, decrypted + i * 16);
    }
    end = clock();
    double decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("大数据处理 (1MB):\n");
    printf("  加密: %.6f秒, %.2f MB/s\n", 
           encrypt_time, data_size / (encrypt_time * 1024 * 1024));
    printf("  解密: %.6f秒, %.2f MB/s\n", 
           decrypt_time, data_size / (decrypt_time * 1024 * 1024));
    
    free(large_data);
    free(encrypted);
    free(decrypted);
    printf("\n");
}

int main() {
    printf("SM4优化对比示例\n");
    printf("================\n\n");
    
    // 检查CPU特性
    printf("CPU特性检查:\n");
    printf("  AESNI: %s\n", sm4_aesni_available() ? "✓ 支持" : "✗ 不支持");
    printf("  GFNI: %s\n", sm4_gfni_available() ? "✓ 支持" : "✗ 不支持");
    printf("  VPROLD: %s\n", sm4_vprold_available() ? "✓ 支持" : "✗ 不支持");
    printf("\n");
    
    // 性能对比测试
    printf("=== 性能对比测试 ===\n");
    printf("测试配置: 100,000次迭代\n\n");
    
    benchmark_optimization("基本实现", 
                          sm4_encrypt_basic, sm4_decrypt_basic, 1);
    
    benchmark_optimization("T-table优化", 
                          sm4_encrypt_ttable, sm4_decrypt_ttable, 1);
    
    benchmark_optimization("AESNI优化", 
                          sm4_encrypt_aesni, sm4_decrypt_aesni, 
                          sm4_aesni_available());
    
    benchmark_optimization("GFNI优化", 
                          sm4_encrypt_gfni, sm4_decrypt_gfni, 
                          sm4_gfni_available());
    
    benchmark_optimization("VPROLD优化", 
                          sm4_encrypt_vprold, sm4_decrypt_vprold, 
                          sm4_vprold_available());
    
    // 批量处理测试
    benchmark_batch_processing();
    
    // 内存使用测试
    benchmark_memory_usage();
    
    printf("优化对比测试完成！\n");
    return 0;
} 