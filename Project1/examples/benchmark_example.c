#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
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

// 获取高精度时间
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// 性能测试结构
typedef struct {
    const char *name;
    void (*encrypt_func)(const uint8_t*, const uint8_t*, uint8_t*);
    void (*decrypt_func)(const uint8_t*, const uint8_t*, uint8_t*);
    int available;
    double encrypt_mbps;
    double decrypt_mbps;
    double encrypt_latency;
    double decrypt_latency;
} benchmark_result_t;

// 详细性能测试
benchmark_result_t benchmark_implementation(const char *name,
                                         void (*encrypt_func)(const uint8_t*, const uint8_t*, uint8_t*),
                                         void (*decrypt_func)(const uint8_t*, const uint8_t*, uint8_t*),
                                         int available) {
    benchmark_result_t result = {0};
    result.name = name;
    result.encrypt_func = encrypt_func;
    result.decrypt_func = decrypt_func;
    result.available = available;
    
    if (!available) {
        return result;
    }
    
    const int warmup_iterations = 10000;
    const int test_iterations = 100000;
    const int latency_iterations = 1000000;
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 预热
    for (int i = 0; i < warmup_iterations; i++) {
        encrypt_func(test_key, test_plaintext, ciphertext);
        decrypt_func(test_key, ciphertext, decrypted);
    }
    
    // 吞吐量测试 - 加密
    double start_time = get_time();
    for (int i = 0; i < test_iterations; i++) {
        encrypt_func(test_key, test_plaintext, ciphertext);
    }
    double end_time = get_time();
    double encrypt_time = end_time - start_time;
    
    // 吞吐量测试 - 解密
    start_time = get_time();
    for (int i = 0; i < test_iterations; i++) {
        decrypt_func(test_key, ciphertext, decrypted);
    }
    end_time = get_time();
    double decrypt_time = end_time - start_time;
    
    // 计算吞吐量 (MB/s)
    double total_bytes = test_iterations * 16.0;
    result.encrypt_mbps = total_bytes / (encrypt_time * 1024 * 1024);
    result.decrypt_mbps = total_bytes / (decrypt_time * 1024 * 1024);
    
    // 延迟测试 - 加密
    start_time = get_time();
    for (int i = 0; i < latency_iterations; i++) {
        encrypt_func(test_key, test_plaintext, ciphertext);
    }
    end_time = get_time();
    double encrypt_latency_time = end_time - start_time;
    
    // 延迟测试 - 解密
    start_time = get_time();
    for (int i = 0; i < latency_iterations; i++) {
        decrypt_func(test_key, ciphertext, decrypted);
    }
    end_time = get_time();
    double decrypt_latency_time = end_time - start_time;
    
    // 计算延迟 (纳秒)
    result.encrypt_latency = (encrypt_latency_time / latency_iterations) * 1000000000.0;
    result.decrypt_latency = (decrypt_latency_time / latency_iterations) * 1000000000.0;
    
    return result;
}

// 打印性能结果
void print_benchmark_results(benchmark_result_t *results, int count) {
    printf("性能基准测试结果\n");
    printf("================\n\n");
    
    printf("%-15s %-12s %-12s %-12s %-12s\n", 
           "实现", "加密(MB/s)", "解密(MB/s)", "加密延迟(ns)", "解密延迟(ns)");
    printf("----------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        if (results[i].available) {
            printf("%-15s %-12.2f %-12.2f %-12.2f %-12.2f\n",
                   results[i].name,
                   results[i].encrypt_mbps,
                   results[i].decrypt_mbps,
                   results[i].encrypt_latency,
                   results[i].decrypt_latency);
        } else {
            printf("%-15s %-12s %-12s %-12s %-12s\n",
                   results[i].name, "不支持", "不支持", "不支持", "不支持");
        }
    }
    printf("\n");
}

// 大数据性能测试
void benchmark_large_data() {
    printf("=== 大数据性能测试 ===\n");
    
    const size_t data_sizes[] = {1024, 10240, 102400, 1048576}; // 1KB, 10KB, 100KB, 1MB
    const int num_sizes = 4;
    
    for (int size_idx = 0; size_idx < num_sizes; size_idx++) {
        size_t data_size = data_sizes[size_idx];
        uint8_t *data = malloc(data_size);
        uint8_t *encrypted = malloc(data_size);
        uint8_t *decrypted = malloc(data_size);
        
        // 初始化数据
        for (size_t i = 0; i < data_size; i++) {
            data[i] = (uint8_t)(i & 0xFF);
        }
        
        const size_t blocks = data_size / 16;
        printf("数据大小: %zu 字节 (%.2f KB)\n", data_size, data_size / 1024.0);
        
        // 测试基本实现
        double start_time = get_time();
        for (size_t i = 0; i < blocks; i++) {
            sm4_encrypt_basic(test_key, data + i * 16, encrypted + i * 16);
        }
        double encrypt_time = get_time() - start_time;
        
        start_time = get_time();
        for (size_t i = 0; i < blocks; i++) {
            sm4_decrypt_basic(test_key, encrypted + i * 16, decrypted + i * 16);
        }
        double decrypt_time = get_time() - start_time;
        
        printf("  基本实现: 加密 %.2f MB/s, 解密 %.2f MB/s\n",
               data_size / (encrypt_time * 1024 * 1024),
               data_size / (decrypt_time * 1024 * 1024));
        
        // 验证正确性
        int correct = (memcmp(data, decrypted, data_size) == 0);
        printf("  正确性: %s\n", correct ? "✓ 通过" : "✗ 失败");
        
        free(data);
        free(encrypted);
        free(decrypted);
        printf("\n");
    }
}

// 内存带宽测试
void benchmark_memory_bandwidth() {
    printf("=== 内存带宽测试 ===\n");
    
    const size_t buffer_size = 16 * 1024 * 1024; // 16MB
    uint8_t *buffer = malloc(buffer_size);
    
    if (!buffer) {
        printf("内存分配失败\n");
        return;
    }
    
    // 初始化缓冲区
    for (size_t i = 0; i < buffer_size; i++) {
        buffer[i] = (uint8_t)(i & 0xFF);
    }
    
            const size_t blocks = buffer_size / 16;
    const int iterations = 10;
    
    printf("缓冲区大小: %zu 字节 (%.2f MB)\n", buffer_size, buffer_size / (1024.0 * 1024.0));
    printf("迭代次数: %d\n\n", iterations);
    
    // 测试不同实现的带宽
    benchmark_result_t results[] = {
        {"基本实现", sm4_encrypt_basic, sm4_decrypt_basic, 1, 0, 0, 0, 0},
        {"T-table", sm4_encrypt_ttable, sm4_decrypt_ttable, 1, 0, 0, 0, 0},
        {"AESNI", sm4_encrypt_aesni, sm4_decrypt_aesni, sm4_aesni_available(), 0, 0, 0, 0},
        {"GFNI", sm4_encrypt_gfni, sm4_decrypt_gfni, sm4_gfni_available(), 0, 0, 0, 0},
        {"VPROLD", sm4_encrypt_vprold, sm4_decrypt_vprold, sm4_vprold_available(), 0, 0, 0, 0}
    };
    
    for (int i = 0; i < 5; i++) {
        if (!results[i].available) {
            printf("%s: 不支持\n", results[i].name);
            continue;
        }
        
        uint8_t *encrypted = malloc(buffer_size);
        uint8_t *decrypted = malloc(buffer_size);
        
        // 加密带宽测试
        double start_time = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            for (size_t j = 0; j < blocks; j++) {
                results[i].encrypt_func(test_key, buffer + j * 16, encrypted + j * 16);
            }
        }
        double encrypt_time = get_time() - start_time;
        
        // 解密带宽测试
        start_time = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            for (size_t j = 0; j < blocks; j++) {
                results[i].decrypt_func(test_key, encrypted + j * 16, decrypted + j * 16);
            }
        }
        double decrypt_time = get_time() - start_time;
        
        double total_data = buffer_size * iterations;
        double encrypt_bandwidth = total_data / (encrypt_time * 1024 * 1024);
        double decrypt_bandwidth = total_data / (decrypt_time * 1024 * 1024);
        
        printf("%s: 加密 %.2f MB/s, 解密 %.2f MB/s\n",
               results[i].name, encrypt_bandwidth, decrypt_bandwidth);
        
        free(encrypted);
        free(decrypted);
    }
    
    free(buffer);
    printf("\n");
}

// CPU利用率测试
void benchmark_cpu_utilization() {
    printf("=== CPU利用率测试 ===\n");
    
    const int test_duration = 5; // 5秒
    const int iterations_per_second = 1000000; // 每秒100万次
    
    printf("测试持续时间: %d 秒\n", test_duration);
    printf("目标频率: %d 次/秒\n", iterations_per_second);
    printf("\n");
    
    // 测试基本实现
    printf("基本实现测试:\n");
    clock_t start_clock = clock();
    double start_time = get_time();
    
    int iterations = 0;
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    while (get_time() - start_time < test_duration) {
        sm4_encrypt_basic(test_key, test_plaintext, ciphertext);
        sm4_decrypt_basic(test_key, ciphertext, decrypted);
        iterations++;
    }
    
    double end_time = get_time();
    clock_t end_clock = clock();
    
    double actual_duration = end_time - start_time;
    double cpu_time = ((double)(end_clock - start_clock)) / CLOCKS_PER_SEC;
    double actual_frequency = iterations / actual_duration;
    double cpu_utilization = (cpu_time / actual_duration) * 100.0;
    
    printf("  实际频率: %.2f 次/秒\n", actual_frequency);
    printf("  CPU利用率: %.2f%%\n", cpu_utilization);
    printf("  总迭代次数: %d\n", iterations);
    printf("\n");
}

int main() {
    printf("SM4性能基准测试\n");
    printf("==============\n\n");
    
    // 检查CPU特性
    printf("CPU特性检查:\n");
    printf("  AESNI: %s\n", sm4_aesni_available() ? "✓ 支持" : "✗ 不支持");
    printf("  GFNI: %s\n", sm4_gfni_available() ? "✓ 支持" : "✗ 不支持");
    printf("  VPROLD: %s\n", sm4_vprold_available() ? "✓ 支持" : "✗ 不支持");
    printf("\n");
    
    // 详细性能测试
    benchmark_result_t results[] = {
        benchmark_implementation("基本实现", sm4_encrypt_basic, sm4_decrypt_basic, 1),
        benchmark_implementation("T-table", sm4_encrypt_ttable, sm4_decrypt_ttable, 1),
        benchmark_implementation("AESNI", sm4_encrypt_aesni, sm4_decrypt_aesni, sm4_aesni_available()),
        benchmark_implementation("GFNI", sm4_encrypt_gfni, sm4_decrypt_gfni, sm4_gfni_available()),
        benchmark_implementation("VPROLD", sm4_encrypt_vprold, sm4_decrypt_vprold, sm4_vprold_available())
    };
    
    print_benchmark_results(results, 5);
    
    // 大数据性能测试
    benchmark_large_data();
    
    // 内存带宽测试
    benchmark_memory_bandwidth();
    
    // CPU利用率测试
    benchmark_cpu_utilization();
    
    printf("性能基准测试完成！\n");
    return 0;
} 