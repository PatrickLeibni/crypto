#include "sm3.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 以十六进制格式打印摘要
void sm3_print_digest(const uint8_t *digest) {
    if (!digest) return;
    
    for (int i = 0; i < SM3_DIGEST_SIZE; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}

// 将十六进制字符串转换为字节数组
void sm3_hex_to_bytes(const char *hex, uint8_t *bytes, size_t len) {
    if (!hex || !bytes) return;
    
    for (size_t i = 0; i < len; i++) {
        char hex_byte[3] = {hex[i * 2], hex[i * 2 + 1], '\0'};
        bytes[i] = (uint8_t)strtol(hex_byte, NULL, 16);
    }
}

// 将字节数组转换为十六进制字符串
void sm3_bytes_to_hex(const uint8_t *bytes, char *hex, size_t len) {
    if (!bytes || !hex) return;
    
    for (size_t i = 0; i < len; i++) {
        snprintf(hex + i * 2, 3, "%02x", bytes[i]);
    }
    hex[len * 2] = '\0';
}

// 性能基准测试函数
sm3_performance_result_t sm3_benchmark(const uint8_t *data, size_t len, int iterations) {
    sm3_performance_result_t result = {0};
    result.data_size = len;
    
    clock_t start, end;
    double cpu_time_used;
    
    // Benchmark基本实现
    start = clock();
    for (int i = 0; i < iterations; i++) {
        uint8_t digest[SM3_DIGEST_SIZE];
        sm3_hash(data, len, digest);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    result.basic_time = cpu_time_used;
    
    // Benchmark优化实现
    start = clock();
    for (int i = 0; i < iterations; i++) {
        uint8_t digest[SM3_DIGEST_SIZE];
        sm3_hash_optimized(data, len, digest);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    result.optimized_time = cpu_time_used;
    
    // Benchmark SIMD 实现
    start = clock();
    for (int i = 0; i < iterations; i++) {
        uint8_t digest[SM3_DIGEST_SIZE];
        sm3_hash_simd(data, len, digest);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    result.simd_time = cpu_time_used;
    
    return result;
}

// 打印性能测试结果
void sm3_print_performance_results(const sm3_performance_result_t *result) {
    if (!result) return;
    
    printf("=== SM3 Performance Benchmark ===\n");
    printf("Data size: %zu bytes\n", result->data_size);
    printf("Basic implementation: %.6f seconds\n", result->basic_time);
    printf("Optimized implementation: %.6f seconds\n", result->optimized_time);
    printf("SIMD implementation: %.6f seconds\n", result->simd_time);
    
    if (result->basic_time > 0) {
        double speedup_opt = result->basic_time / result->optimized_time;
        double speedup_simd = result->basic_time / result->simd_time;
        printf("Optimized speedup: %.2fx\n", speedup_opt);
        printf("SIMD speedup: %.2fx\n", speedup_simd);
    }
    printf("\n");
}

// 生成随机测试数据
void sm3_generate_random_data(uint8_t *data, size_t len) {
    if (!data) return;
    
    srand(time(NULL));
    for (size_t i = 0; i < len; i++) {
        data[i] = (uint8_t)(rand() % 256);
    }
}

// 测试 SM3 实现的正确性
int sm3_test_correctness() {
    printf("=== SM3 Correctness Test ===\n");
    
    // 测试向量（示例）
    const char *test_messages[] = {
        "",
        "a",
        "abc",
        "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    };
    
    const char *expected_hashes[] = {
        "1ab21d8355cfa17f8e6119483c47424a83c63f93189d909dd812a0e2ae2817b",
        "623476ac18f65d290161e318e87e393817f44f4a623d2a75f7188e8b30809c4",
        "66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e",
        "ad2b79d8cb8783a8035a264b3bd2cb2a3a131d98f8ce500a00341af40025c15",
        "b80fe97a4da24af53d2c3f7d88648839d9b142b1eb7bd813c4508a4b3ffdd8c",
        "5f525d580fadf1624f5bb3badc466ed88e9b71ab0d4d0e4e8cfd251b2e5f5a5",
        "b8ac4203969bde27434ce667b0adbf3439ee97e416e73cb96f4431f478ac5310"
    };
    
    int num_tests = sizeof(test_messages) / sizeof(test_messages[0]);
    int passed = 0;
    
    for (int i = 0; i < num_tests; i++) {
        uint8_t digest[SM3_DIGEST_SIZE];
        uint8_t expected[SM3_DIGEST_SIZE];
        char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
        
        // 计算哈希
        sm3_hash((uint8_t*)test_messages[i], strlen(test_messages[i]), digest);
        
        // 将期望的哈希值转换为字节
        sm3_hex_to_bytes(expected_hashes[i], expected, SM3_DIGEST_SIZE);
        
        // 将计算的哈希值转换为十六进制进行比较
        sm3_bytes_to_hex(digest, hex_digest, SM3_DIGEST_SIZE);
        
        // 对比
        if (memcmp(digest, expected, SM3_DIGEST_SIZE) == 0) {
            printf("✓ Test %d passed\n", i + 1);
            passed++;
        } else {
            printf("✗ Test %d failed\n", i + 1);
            printf("  Expected: %s\n", expected_hashes[i]);
            printf("  Got:      %s\n", hex_digest);
        }
    }
    
    printf("Correctness test: %d/%d passed\n\n", passed, num_tests);
    return passed == num_tests;
}

// 测试优化实现与基础实现的一致性
int sm3_test_optimized_correctness() {
    printf("=== SM3 Optimized Implementation Test ===\n");
    
    // 生成随机测试数据
    uint8_t test_data[1024];
    sm3_generate_random_data(test_data, sizeof(test_data));
    
    uint8_t basic_digest[SM3_DIGEST_SIZE];
    uint8_t optimized_digest[SM3_DIGEST_SIZE];
    
    // 计算哈希值
    sm3_hash(test_data, sizeof(test_data), basic_digest);
    sm3_hash_optimized(test_data, sizeof(test_data), optimized_digest);
    
    // 对比结果
    if (memcmp(basic_digest, optimized_digest, SM3_DIGEST_SIZE) == 0) {
        printf("✓ Optimized implementation produces correct results\n");
        return 1;
    } else {
        printf("✗ Optimized implementation produces incorrect results\n");
        printf("Basic digest: ");
        sm3_print_digest(basic_digest);
        printf("Optimized digest: ");
        sm3_print_digest(optimized_digest);
        return 0;
    }
}

// 综合性能测试
void sm3_comprehensive_performance_test() {
    printf("=== SM3 Comprehensive Performance Test ===\n");
    
    const size_t test_sizes[] = {64, 256, 1024, 4096, 16384, 65536};
    const int iterations = 1000;
    
    for (size_t i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); i++) {
        size_t data_size = test_sizes[i];
        uint8_t *test_data = (uint8_t*)malloc(data_size);
        
        if (!test_data) {
            printf("Failed to allocate memory for test data\n");
            continue;
        }
        
        sm3_generate_random_data(test_data, data_size);
        
        sm3_performance_result_t result = sm3_benchmark(test_data, data_size, iterations);
        sm3_print_performance_results(&result);
        
        free(test_data);
    }
}

// 工具函数演示
void sm3_utils_demo() {
    printf("=== SM3 Utilities Demo ===\n");
    
    // 测试十六进制转换
    const char *test_hex = "1ab21d8355cfa17f8e6119483c47424a83c63f93189d909dd812a0e2ae2817b";
    uint8_t test_bytes[SM3_DIGEST_SIZE];
    char converted_hex[SM3_DIGEST_SIZE * 2 + 1];
    
    sm3_hex_to_bytes(test_hex, test_bytes, SM3_DIGEST_SIZE);
    sm3_bytes_to_hex(test_bytes, converted_hex, SM3_DIGEST_SIZE);
    
    printf("Original hex: %s\n", test_hex);
    printf("Converted hex: %s\n", converted_hex);
    printf("Match: %s\n\n", strcmp(test_hex, converted_hex) == 0 ? "✓" : "✗");
    
    printf("Test digest: ");
    sm3_print_digest(test_bytes);
    
    sm3_test_correctness();
    
    sm3_test_optimized_correctness();
    
    sm3_comprehensive_performance_test();
} 