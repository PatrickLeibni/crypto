#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "../include/sm3.h"

// 性能测试配置
#define NUM_ITERATIONS 10000
#define SMALL_DATA_SIZE 64
#define MEDIUM_DATA_SIZE 1024
#define LARGE_DATA_SIZE 10240

// 获取当前时间（微秒）
double get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// 生成随机数据
void generate_random_data(uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] = rand() % 256;
    }
}

void test_basic_performance() {
    printf("=== 基本性能测试 ===\n");
    
    uint8_t *data = malloc(LARGE_DATA_SIZE);
    uint8_t digest[SM3_DIGEST_SIZE];
    
    generate_random_data(data, LARGE_DATA_SIZE);
    
    // 测试基本实现
    double start_time = get_time_us();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sm3_hash(data, LARGE_DATA_SIZE, digest);
    }
    double end_time = get_time_us();
    double basic_time = (end_time - start_time) / NUM_ITERATIONS;
    
    printf("基本实现: %.2f 微秒/次 (%.2f MB/s)\n", 
           basic_time, (LARGE_DATA_SIZE / 1024.0 / 1024.0) / (basic_time / 1000000.0));
    
    // 测试优化版实现
    start_time = get_time_us();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sm3_hash_optimized(data, LARGE_DATA_SIZE, digest);
    }
    end_time = get_time_us();
    double optimized_time = (end_time - start_time) / NUM_ITERATIONS;
    
    printf("优化版实现: %.2f 微秒/次 (%.2f MB/s)\n", 
           optimized_time, (LARGE_DATA_SIZE / 1024.0 / 1024.0) / (optimized_time / 1000000.0));
    
    free(data);
    printf("\n");
}

void test_data_size_performance() {
    printf("=== 不同数据大小性能测试 ===\n");
    
    const int data_sizes[] = {SMALL_DATA_SIZE, MEDIUM_DATA_SIZE, LARGE_DATA_SIZE};
    const char *size_names[] = {"小数据(64B)", "中数据(1KB)", "大数据(10KB)"};
    const int num_sizes = sizeof(data_sizes) / sizeof(data_sizes[0]);
    
    uint8_t *data = malloc(LARGE_DATA_SIZE);
    uint8_t digest[SM3_DIGEST_SIZE];
    
    generate_random_data(data, LARGE_DATA_SIZE);
    
    printf("%-15s %-15s %-15s\n", 
           "数据大小", "基本实现", "优化版");
    printf("----------------------------------------\n");
    
    for (int s = 0; s < num_sizes; s++) {
        int size = data_sizes[s];
        printf("%-15s", size_names[s]);
        
        // 测试基本实现
        double start_time = get_time_us();
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            sm3_hash(data, size, digest);
        }
        double end_time = get_time_us();
        double basic_time = (end_time - start_time) / NUM_ITERATIONS;
        printf(" %-15.2f", basic_time);
        
        // 测试优化版实现
        start_time = get_time_us();
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            sm3_hash_optimized(data, size, digest);
        }
        end_time = get_time_us();
        double optimized_time = (end_time - start_time) / NUM_ITERATIONS;
        printf(" %-15.2f", optimized_time);
        
        printf("\n");
    }
    
    free(data);
    printf("\n");
}

void test_simd_performance() {
    printf("=== SIMD性能测试 ===\n");
    
    uint8_t data[LARGE_DATA_SIZE];
    uint8_t digest[SM3_DIGEST_SIZE];
    
    generate_random_data(data, LARGE_DATA_SIZE);
    
    // 测试SIMD实现
    double start_time = get_time_us();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sm3_hash_simd(data, LARGE_DATA_SIZE, digest);
    }
    double end_time = get_time_us();
    double simd_time = (end_time - start_time) / NUM_ITERATIONS;
    
    printf("SIMD实现: %.2f 微秒/次 (%.2f MB/s)\n", 
           simd_time, (LARGE_DATA_SIZE / 1024.0 / 1024.0) / (simd_time / 1000000.0));
    
    // 与优化版比较
    start_time = get_time_us();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sm3_hash_optimized(data, LARGE_DATA_SIZE, digest);
    }
    end_time = get_time_us();
    double optimized_time = (end_time - start_time) / NUM_ITERATIONS;
    
    printf("优化版实现: %.2f 微秒/次 (%.2f MB/s)\n", 
           optimized_time, (LARGE_DATA_SIZE / 1024.0 / 1024.0) / (optimized_time / 1000000.0));
    
    if (simd_time < optimized_time) {
        printf("✓ SIMD实现比优化版快 %.1f%%\n", 
               ((optimized_time - simd_time) / optimized_time) * 100);
    } else {
        printf("✗ SIMD实现比优化版慢 %.1f%%\n", 
               ((simd_time - optimized_time) / optimized_time) * 100);
    }
    
    printf("\n");
}

void test_length_extension_performance() {
    printf("=== 长度扩展攻击性能测试 ===\n");
    
    const char *original_message = "Hello, World!";
    const char *extension = "This is an extension message for testing performance";
    
    uint8_t original_digest[SM3_DIGEST_SIZE];
    uint8_t new_digest[SM3_DIGEST_SIZE];
    
    // 计算原始哈希
    sm3_hash((uint8_t*)original_message, strlen(original_message), original_digest);
    
    // 测试攻击性能
    double start_time = get_time_us();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sm3_length_extension_attack(
            original_digest,
            (uint8_t*)original_message,
            strlen(original_message),
            (uint8_t*)extension,
            strlen(extension),
            new_digest
        );
    }
    double end_time = get_time_us();
    double attack_time = (end_time - start_time) / NUM_ITERATIONS;
    
    printf("长度扩展攻击: %.2f 微秒/次\n", attack_time);
    
    // 比较正常哈希计算时间
    start_time = get_time_us();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        char combined_message[1024];
        snprintf(combined_message, sizeof(combined_message), "%s%s", original_message, extension);
        sm3_hash((uint8_t*)combined_message, strlen(combined_message), new_digest);
    }
    end_time = get_time_us();
    double normal_time = (end_time - start_time) / NUM_ITERATIONS;
    
    printf("正常哈希计算: %.2f 微秒/次\n", normal_time);
    
    if (attack_time < normal_time) {
        printf("✓ 攻击比正常计算快 %.1f%%\n", 
               ((normal_time - attack_time) / normal_time) * 100);
    } else {
        printf("✗ 攻击比正常计算慢 %.1f%%\n", 
               ((attack_time - normal_time) / normal_time) * 100);
    }
    
    printf("\n");
}

void test_merkle_tree_performance() {
    printf("=== Merkle树性能测试 ===\n");
    
    const int tree_sizes[] = {100, 1000, 10000};
    const int num_sizes = sizeof(tree_sizes) / sizeof(tree_sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int leaf_count = tree_sizes[s];
        printf("测试 %d 个叶子节点的Merkle树...\n", leaf_count);
        
        // 创建叶子哈希
        uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
        for (int i = 0; i < leaf_count; i++) {
            leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
            char message[64];
            snprintf(message, sizeof(message), "perf_test_leaf_%d", i);
            sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        }
        
        // 测量树创建时间
        double start_time = get_time_us();
        merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
        double end_time = get_time_us();
        double create_time = end_time - start_time;
        
        if (tree) {
            printf("  树创建时间: %.2f 毫秒\n", create_time / 1000.0);
            
            // 测量证明创建时间
            start_time = get_time_us();
            merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, 0);
            end_time = get_time_us();
            double proof_time = end_time - start_time;
            
            if (proof) {
                printf("  证明创建时间: %.2f 微秒\n", proof_time);
                
                // 测量验证时间
                start_time = get_time_us();
                int valid = merkle_tree_verify_existence_proof(tree, proof);
                end_time = get_time_us();
                double verify_time = end_time - start_time;
                
                printf("  验证时间: %.2f 微秒\n", verify_time);
                printf("  验证结果: %s\n", valid ? "成功" : "失败");
                
                merkle_proof_destroy(proof);
            }
            
            merkle_tree_destroy(tree);
        }
        
        // 清理
        for (int i = 0; i < leaf_count; i++) {
            free(leaf_hashes[i]);
        }
        free(leaf_hashes);
        
        printf("\n");
    }
}

int main() {
    printf("开始性能测试...\n\n");
    
    // 设置随机种子
    srand(time(NULL));
    
    test_basic_performance();
    test_data_size_performance();
    test_simd_performance();
    test_length_extension_performance();
    test_merkle_tree_performance();
    
    printf("性能测试完成！\n");
    return 0;
} 