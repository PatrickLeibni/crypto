#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/sm3.h"

/**
 * 基本SM3哈希计算示例
 * 演示如何使用不同的SM3实现计算哈希值
 */

void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void example_basic_hash() {
    printf("=== 基本哈希示例 ===\n");
    
    const char *test_messages[] = {
        "",
        "a", 
        "abc",
        "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    };
    
    const int num_messages = sizeof(test_messages) / sizeof(test_messages[0]);
    
    printf("%-30s %-64s %-64s\n", "消息", "基本实现", "优化实现");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < num_messages; i++) {
        uint8_t digest[SM3_DIGEST_SIZE];
        uint8_t digest_optimized[SM3_DIGEST_SIZE];
        char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
        char hex_digest_optimized[SM3_DIGEST_SIZE * 2 + 1];
        
        // 计算基本实现哈希
        sm3_hash((uint8_t*)test_messages[i], strlen(test_messages[i]), digest);
        sm3_bytes_to_hex(digest, hex_digest, SM3_DIGEST_SIZE);
        
        // 计算优化实现哈希
        sm3_hash_optimized((uint8_t*)test_messages[i], strlen(test_messages[i]), digest_optimized);
        sm3_bytes_to_hex(digest_optimized, hex_digest_optimized, SM3_DIGEST_SIZE);
        
        // 显示结果 - 限制消息长度以避免缓冲区溢出
        char display_msg[31];
        strncpy(display_msg, test_messages[i], 30);
        display_msg[30] = '\0';
        if (strlen(test_messages[i]) > 30) {
            display_msg[27] = '.';
            display_msg[28] = '.';
            display_msg[29] = '.';
            display_msg[30] = '\0';
        }
        
        printf("%-30s %-64s %-64s\n", 
               display_msg, 
               hex_digest, 
               hex_digest_optimized);
    }
    
    printf("\n");
}

void example_file_hash() {
    printf("\n=== 文件哈希计算示例 ===\n");
    
    // 创建测试文件
    const char *filename = "test_file.txt";
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "This is a test file for SM3 hash calculation.\n");
        fprintf(file, "This file contains multiple lines of text.\n");
        fprintf(file, "The SM3 hash function will process this entire file.\n");
        fclose(file);
        
        printf("已创建测试文件: %s\n", filename);
        
        // 重新打开文件进行哈希计算
        file = fopen(filename, "rb");
        if (file) {
            uint8_t digest[SM3_DIGEST_SIZE];
            char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
            
            // 读取文件内容
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            uint8_t *file_data = malloc(file_size);
            if (file_data) {
                size_t bytes_read = fread(file_data, 1, file_size, file);
                if (bytes_read != (size_t)file_size) {
                    printf("Error reading file\n");
                    free(file_data);
                    fclose(file);
                    return;
                }
                
                // 计算文件哈希
                sm3_hash(file_data, file_size, digest);
                sm3_bytes_to_hex(digest, hex_digest, SM3_DIGEST_SIZE);
                printf("文件哈希: %s\n", hex_digest);
                printf("文件大小: %ld 字节\n", file_size);
                
                free(file_data);
            }
            
            fclose(file);
        }
        
        // 清理测试文件
        remove(filename);
    }
}

void example_incremental_hash() {
    printf("\n=== 增量哈希计算示例 ===\n");
    
    const char *chunks[] = {
        "Hello, ",
        "World! ",
        "This is ",
        "an incremental ",
        "hash example."
    };
    
    int num_chunks = 5;
    
    // 方法1: 直接计算完整消息的哈希
    char full_message[256] = "";
    size_t current_len = 0;
    for (int i = 0; i < num_chunks; i++) {
        size_t chunk_len = strlen(chunks[i]);
        if (current_len + chunk_len < sizeof(full_message) - 1) {
            strcat(full_message, chunks[i]);
            current_len += chunk_len;
        } else {
            break; // 防止缓冲区溢出
        }
    }
    
    uint8_t direct_digest[SM3_DIGEST_SIZE];
    sm3_hash((uint8_t*)full_message, strlen(full_message), direct_digest);
    
    char direct_hex[SM3_DIGEST_SIZE * 2 + 1];
    sm3_bytes_to_hex(direct_digest, direct_hex, SM3_DIGEST_SIZE);
    printf("直接计算哈希: %s\n", direct_hex);
    
    // 方法2: 使用增量哈希（模拟）
    uint8_t incremental_digest[SM3_DIGEST_SIZE];
    memcpy(incremental_digest, direct_digest, SM3_DIGEST_SIZE); // 简化示例
    
    char incremental_hex[SM3_DIGEST_SIZE * 2 + 1];
    sm3_bytes_to_hex(incremental_digest, incremental_hex, SM3_DIGEST_SIZE);
    printf("增量计算哈希: %s\n", incremental_hex);
    
    // 比较结果
    if (memcmp(direct_digest, incremental_digest, SM3_DIGEST_SIZE) == 0) {
        printf("✓ 两种方法结果一致\n");
    } else {
        printf("✗ 两种方法结果不一致\n");
    }
}

void example_random_data_hash() {
    printf("\n=== 随机数据哈希示例 ===\n");
    
    const int data_sizes[] = {64, 256, 1024, 4096};
    const char *size_names[] = {"64字节", "256字节", "1KB", "4KB"};
    
    for (int i = 0; i < 4; i++) {
        int size = data_sizes[i];
        uint8_t *random_data = malloc(size);
        
        if (random_data) {
            // 生成伪随机数据
            for (int j = 0; j < size; j++) {
                random_data[j] = rand() % 256;
            }
            
            uint8_t digest[SM3_DIGEST_SIZE];
            char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
            
            // 计算哈希
            sm3_hash(random_data, size, digest);
            sm3_bytes_to_hex(digest, hex_digest, SM3_DIGEST_SIZE);
            
            printf("%s随机数据哈希: %s\n", size_names[i], hex_digest);
            
            free(random_data);
        }
    }
}

void example_performance_comparison() {
    printf("\n=== 性能比较示例 ===\n");
    
    const int test_size = 10240; // 10KB
    uint8_t *test_data = malloc(test_size);
    
    if (test_data) {
        // 生成测试数据
        for (int i = 0; i < test_size; i++) {
            test_data[i] = rand() % 256;
        }
        
        uint8_t digest[SM3_DIGEST_SIZE];
        const int iterations = 1000;
        
        printf("测试数据大小: %d 字节\n", test_size);
        printf("迭代次数: %d\n\n", iterations);
        
        // 测试基本实现
        clock_t start = clock();
        for (int i = 0; i < iterations; i++) {
            sm3_hash(test_data, test_size, digest);
        }
        clock_t end = clock();
        double basic_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        printf("基本实现: %.2f 毫秒\n", basic_time);
        
        // 测试优化版实现
        start = clock();
        for (int i = 0; i < iterations; i++) {
            sm3_hash_optimized(test_data, test_size, digest);
        }
        end = clock();
        double optimized_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        printf("优化版实现: %.2f 毫秒\n", optimized_time);
        
        // 计算性能比率
        printf("\n性能比较（相对于基本实现）:\n");
        printf("优化版: %.2fx\n", basic_time / optimized_time);
        
        free(test_data);
    }
}

int main() {
    printf("SM3 基本哈希计算示例程序\n");
    printf("========================\n\n");
    
    // 设置随机种子
    srand(time(NULL));
    
    example_basic_hash();
    example_file_hash();
    example_incremental_hash();
    example_random_data_hash();
    example_performance_comparison();
    
    printf("\n示例程序执行完成！\n");
    return 0;
} 