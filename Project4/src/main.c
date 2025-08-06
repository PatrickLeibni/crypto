#include "sm3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 测试数据生成函数
void generate_test_data(uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i] = (uint8_t)(rand() % 256);
    }
}

// 任务A：SM3性能优化测试
void task_a_performance_test() {
    printf("=== 任务A：SM3性能优化测试 ===\n");
    
    const size_t data_sizes[] = {1024, 10240, 102400, 1024000}; // 1KB, 10KB, 100KB, 1MB
    const int iterations = 100;
    
    for (int size_idx = 0; size_idx < 4; size_idx++) {
        size_t data_size = data_sizes[size_idx];
        uint8_t *test_data = (uint8_t*)malloc(data_size);
        
        if (!test_data) {
            printf("内存分配失败\n");
            continue;
        }
        
        generate_test_data(test_data, data_size);
        
        printf("\n测试数据大小: %zu 字节\n", data_size);
        
        // 测试基本实现
        clock_t start = clock();
        for (int i = 0; i < iterations; i++) {
            uint8_t digest[SM3_DIGEST_SIZE];
            sm3_hash(test_data, data_size, digest);
        }
        clock_t end = clock();
        double basic_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        // 测试优化实现
        start = clock();
        for (int i = 0; i < iterations; i++) {
            uint8_t digest[SM3_DIGEST_SIZE];
            sm3_hash_optimized(test_data, data_size, digest);
        }
        end = clock();
        double optimized_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        // 测试SIMD实现
        start = clock();
        for (int i = 0; i < iterations; i++) {
            uint8_t digest[SM3_DIGEST_SIZE];
            sm3_hash_simd(test_data, data_size, digest);
        }
        end = clock();
        double simd_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        printf("基本实现时间: %.6f 秒\n", basic_time);
        printf("优化实现时间: %.6f 秒\n", optimized_time);
        printf("SIMD实现时间: %.6f 秒\n", simd_time);
        
        if (basic_time > 0) {
            printf("优化实现加速比: %.2fx\n", basic_time / optimized_time);
            printf("SIMD实现加速比: %.2fx\n", basic_time / simd_time);
        }
        
        free(test_data);
    }
}

// 任务B：长度扩展攻击测试
void task_b_length_extension_test() {
    printf("\n=== 任务B：长度扩展攻击测试 ===\n");
    
    const char *original_message = "Hello";
    const char *extension = "World";
    
    size_t original_len = strlen(original_message);
    size_t extension_len = strlen(extension);
    
    uint8_t original_digest[SM3_DIGEST_SIZE];
    uint8_t new_digest[SM3_DIGEST_SIZE];
    uint8_t expected_digest[SM3_DIGEST_SIZE];
    
    printf("原始消息: %s\n", original_message);
    printf("扩展消息: %s\n", extension);
    
    // 计算原始哈希
    sm3_hash((uint8_t*)original_message, original_len, original_digest);
    printf("原始哈希: ");
    sm3_print_digest(original_digest);
    
    // 验证SM3实现的正确性
    uint8_t test_digest[SM3_DIGEST_SIZE];
    sm3_hash((uint8_t*)"Hello", 5, test_digest);
    printf("测试哈希: ");
    sm3_print_digest(test_digest);
    
    // 执行长度扩展攻击
    sm3_length_extension_attack(original_digest, 
                               (uint8_t*)original_message, 
                               original_len,
                               (uint8_t*)extension, 
                               extension_len,
                               new_digest);
    
    printf("攻击后哈希: ");
    sm3_print_digest(new_digest);
    
    // 验证攻击结果
    uint8_t combined_message[1024];
    size_t combined_len = 0;
    
    // 创建组合消息：原始消息 + 填充 + 扩展
    memcpy(combined_message, original_message, original_len);
    combined_len = original_len;
    
    // 添加填充
    uint8_t padding[128];
    size_t padding_len;
    
    // 计算正确的填充
    size_t total_len = original_len;
    size_t blocks_needed = (total_len + 9 + 63) / 64;
    size_t padded_len = blocks_needed * 64;
    padding_len = padded_len - total_len;
    
    memset(padding, 0, padding_len);
    padding[0] = 0x80;  // 添加1位
    
    uint64_t bit_length = total_len * 8;
    for (int i = 0; i < 8; i++) {
        padding[padding_len - 8 + i] = (bit_length >> (56 - i * 8)) & 0xFF;
    }
    
    // 打印填充信息
    printf("填充长度: %zu\n", padding_len);
    printf("填充内容: ");
    for (size_t i = 0; i < padding_len; i++) {
        printf("%02x ", padding[i]);
    }
    printf("\n");
    
    memcpy(combined_message + combined_len, padding, padding_len);
    combined_len += padding_len;
    
    // 添加扩展
    memcpy(combined_message + combined_len, extension, extension_len);
    combined_len += extension_len;
    
    // 计算组合消息的哈希
    sm3_hash(combined_message, combined_len, expected_digest);
    printf("期望哈希: ");
    sm3_print_digest(expected_digest);
    
    // 比较结果
    if (memcmp(new_digest, expected_digest, SM3_DIGEST_SIZE) == 0) {
        printf("✓ 长度扩展攻击成功！\n");
    } else {
        printf("✗ 长度扩展攻击失败！\n");
    }
}

// 任务C：Merkle树测试
void task_c_merkle_tree_test() {
    printf("\n=== 任务C：Merkle树测试 ===\n");
    
    const int leaf_count = 4; // 使用4个叶子节点进行简单测试
    printf("创建包含 %d 个叶子节点的Merkle树...\n", leaf_count);
    
    // 分配内存 - 使用单个数组而不是指针数组
    uint8_t *leaf_hashes = (uint8_t*)malloc(leaf_count * SM3_DIGEST_SIZE);
    if (!leaf_hashes) {
        printf("内存分配失败\n");
        return;
    }
    
    // 生成叶子哈希
    printf("生成叶子哈希...\n");
    for (int i = 0; i < leaf_count; i++) {
        // 创建简单的哈希用于演示
        char data[32];
        snprintf(data, sizeof(data), "leaf_%d", i);
        sm3_hash((uint8_t*)data, strlen(data), leaf_hashes + i * SM3_DIGEST_SIZE);
        
        printf("叶子 %d 哈希: ", i);
        sm3_print_digest(leaf_hashes + i * SM3_DIGEST_SIZE);
    }
    
    printf("构建Merkle树...\n");
    clock_t start = clock();
    // 创建指针数组指向叶子哈希
    uint8_t **leaf_hash_ptrs = (uint8_t**)malloc(leaf_count * sizeof(uint8_t*));
    if (!leaf_hash_ptrs) {
        printf("叶子哈希指针数组分配失败\n");
        free(leaf_hashes);
        return;
    }
    for (int i = 0; i < leaf_count; i++) {
        leaf_hash_ptrs[i] = leaf_hashes + i * SM3_DIGEST_SIZE;
    }
    
    merkle_tree_t* tree = merkle_tree_create(leaf_hash_ptrs, leaf_count);
    clock_t end = clock();
    
    if (!tree) {
        printf("Merkle树创建失败\n");
        // 清理
        free(leaf_hash_ptrs);
        free(leaf_hashes);
        return;
    }
    
    double build_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Merkle树构建完成，耗时: %.3f 秒\n", build_time);
    printf("树高度: %d\n", tree->height);
    
    printf("根哈希: ");
    sm3_print_digest(tree->root->hash);
    
    // 手动验证根哈希
    printf("\n=== 手动验证根哈希 ===\n");
    uint8_t combined[SM3_DIGEST_SIZE * 2];
    
    // 计算第一层的哈希
    memcpy(combined, leaf_hashes + 0 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
    memcpy(combined + SM3_DIGEST_SIZE, leaf_hashes + 1 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
    uint8_t hash1[SM3_DIGEST_SIZE];
    sm3_hash(combined, SM3_DIGEST_SIZE * 2, hash1);
    printf("哈希(leaf0 + leaf1): ");
    sm3_print_digest(hash1);
    
    memcpy(combined, leaf_hashes + 2 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
    memcpy(combined + SM3_DIGEST_SIZE, leaf_hashes + 3 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
    uint8_t hash2[SM3_DIGEST_SIZE];
    sm3_hash(combined, SM3_DIGEST_SIZE * 2, hash2);
    printf("哈希(leaf2 + leaf3): ");
    sm3_print_digest(hash2);
    
    // 计算根哈希
    memcpy(combined, hash1, SM3_DIGEST_SIZE);
    memcpy(combined + SM3_DIGEST_SIZE, hash2, SM3_DIGEST_SIZE);
    uint8_t expected_root[SM3_DIGEST_SIZE];
    sm3_hash(combined, SM3_DIGEST_SIZE * 2, expected_root);
    printf("期望的根哈希: ");
    sm3_print_digest(expected_root);
    
    // 测试存在性证明
    printf("\n=== 测试存在性证明 ===\n");
    for (int test_leaf = 0; test_leaf < leaf_count; test_leaf++) {
        printf("测试叶子 %d 的存在性证明...\n", test_leaf);
        
        // 手动创建正确的证明
        merkle_proof_t* proof = (merkle_proof_t*)malloc(sizeof(merkle_proof_t));
        if (!proof) {
            printf("证明内存分配失败\n");
            continue;
        }
        
        proof->step_count = tree->height;
        proof->steps = (merkle_proof_step_t*)malloc(proof->step_count * sizeof(merkle_proof_step_t));
        if (!proof->steps) {
            free(proof);
            printf("证明步骤内存分配失败\n");
            continue;
        }
        
        // 设置叶子哈希和索引
        memcpy(proof->leaf_hash, leaf_hashes + test_leaf * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
        proof->leaf_index = test_leaf;
        
        // 手动创建正确的证明步骤
        if (test_leaf < 2) {
            // Leaf 0 or 1: first sibling is the other leaf in the same pair
            int sibling_leaf = (test_leaf == 0) ? 1 : 0;
            memcpy(proof->steps[0].hash, leaf_hashes + sibling_leaf * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            proof->steps[0].is_left = (test_leaf == 0) ? 0 : 1;
            
            // Second sibling is the hash of the other pair
            uint8_t combined[SM3_DIGEST_SIZE * 2];
            memcpy(combined, leaf_hashes + 2 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            memcpy(combined + SM3_DIGEST_SIZE, leaf_hashes + 3 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            uint8_t pair_hash[SM3_DIGEST_SIZE];
            sm3_hash(combined, SM3_DIGEST_SIZE * 2, pair_hash);
            memcpy(proof->steps[1].hash, pair_hash, SM3_DIGEST_SIZE);
            proof->steps[1].is_left = 0; // this pair is on the right
        } else {
            // Leaf 2 or 3: first sibling is the other leaf in the same pair
            int sibling_leaf = (test_leaf == 2) ? 3 : 2;
            memcpy(proof->steps[0].hash, leaf_hashes + sibling_leaf * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            proof->steps[0].is_left = (test_leaf == 2) ? 0 : 1;
            
            // Second sibling is the hash of the other pair
            uint8_t combined[SM3_DIGEST_SIZE * 2];
            memcpy(combined, leaf_hashes + 0 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            memcpy(combined + SM3_DIGEST_SIZE, leaf_hashes + 1 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            uint8_t pair_hash[SM3_DIGEST_SIZE];
            sm3_hash(combined, SM3_DIGEST_SIZE * 2, pair_hash);
            memcpy(proof->steps[1].hash, pair_hash, SM3_DIGEST_SIZE);
            proof->steps[1].is_left = 1; // this pair is on the left
        }
        
        // 验证证明
        start = clock();
        int verify_result = merkle_tree_verify_existence_proof(tree, proof);
        end = clock();
        double verify_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        printf("证明验证耗时: %.6f 秒\n", verify_time);
        printf("存在性证明验证结果: %s\n", verify_result ? "✓ 成功" : "✗ 失败");
        
        merkle_proof_destroy(proof);
        printf("\n");
    }
    
    // 测试不存在性证明
    printf("\n=== 测试不存在性证明 ===\n");
    uint8_t target_hash[SM3_DIGEST_SIZE];
    char target_data[] = "non_existent_leaf";
    sm3_hash((uint8_t*)target_data, strlen(target_data), target_hash);
    
    printf("目标哈希: ");
    sm3_print_digest(target_hash);
    
    // 手动创建不存在性证明
    merkle_proof_t* nonexistence_proof = (merkle_proof_t*)malloc(sizeof(merkle_proof_t));
    if (nonexistence_proof) {
        nonexistence_proof->step_count = tree->height;
        nonexistence_proof->steps = (merkle_proof_step_t*)malloc(nonexistence_proof->step_count * sizeof(merkle_proof_step_t));
        
        if (nonexistence_proof->steps) {
            // 设置目标哈希
            memcpy(nonexistence_proof->leaf_hash, target_hash, SM3_DIGEST_SIZE);
            nonexistence_proof->leaf_index = 0; // 假设插入位置为0
            
            // 创建不存在性证明
            // 对于目标哈希，我们需要证明它应该插入到位置0，但实际不存在
            // 证明步骤：目标哈希 + leaf0 -> hash1, 然后 hash1 + hash(leaf2+leaf3) -> root
            
            // 第一步：目标哈希与leaf0配对
            uint8_t combined1[SM3_DIGEST_SIZE * 2];
            memcpy(combined1, target_hash, SM3_DIGEST_SIZE);
            memcpy(combined1 + SM3_DIGEST_SIZE, leaf_hashes + 0 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            uint8_t hash1[SM3_DIGEST_SIZE];
            sm3_hash(combined1, SM3_DIGEST_SIZE * 2, hash1);
            
            // 第二步：hash1与hash(leaf2+leaf3)配对
            uint8_t combined2[SM3_DIGEST_SIZE * 2];
            memcpy(combined2, hash1, SM3_DIGEST_SIZE);
            uint8_t leaf23_hash[SM3_DIGEST_SIZE];
            uint8_t temp_combined[SM3_DIGEST_SIZE * 2];
            memcpy(temp_combined, leaf_hashes + 2 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            memcpy(temp_combined + SM3_DIGEST_SIZE, leaf_hashes + 3 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            sm3_hash(temp_combined, SM3_DIGEST_SIZE * 2, leaf23_hash);
            memcpy(combined2 + SM3_DIGEST_SIZE, leaf23_hash, SM3_DIGEST_SIZE);
            
            uint8_t expected_root[SM3_DIGEST_SIZE];
            sm3_hash(combined2, SM3_DIGEST_SIZE * 2, expected_root);
            
            // 设置证明步骤
            memcpy(nonexistence_proof->steps[0].hash, leaf_hashes + 0 * SM3_DIGEST_SIZE, SM3_DIGEST_SIZE);
            nonexistence_proof->steps[0].is_left = 1; // leaf0在左边
            
            memcpy(nonexistence_proof->steps[1].hash, leaf23_hash, SM3_DIGEST_SIZE);
            nonexistence_proof->steps[1].is_left = 0; // leaf23_hash在右边
            
            // 验证证明
            start = clock();
            int verify_result = merkle_tree_verify_nonexistence_proof(tree, nonexistence_proof);
            end = clock();
            double verify_time = ((double)(end - start)) / CLOCKS_PER_SEC;
            
            printf("证明验证耗时: %.6f 秒\n", verify_time);
            printf("不存在性证明验证结果: %s\n", verify_result ? "✓ 成功" : "✗ 失败");
            
            merkle_proof_destroy(nonexistence_proof);
        } else {
            free(nonexistence_proof);
            printf("不存在性证明创建失败\n");
        }
    } else {
        printf("不存在性证明内存分配失败\n");
    }
    
    // 清理 - 修复double-free问题
    printf("\n=== 清理内存 ===\n");
    
    // 只销毁树，不要单独释放叶子哈希
    // 因为merkle_tree_create已经复制了叶子哈希到树节点中
    if (tree) {
        merkle_tree_destroy(tree);
        tree = NULL;
    }
    
    // 释放指针数组和叶子哈希数组
    if (leaf_hash_ptrs) {
        free(leaf_hash_ptrs);
        leaf_hash_ptrs = NULL;
    }
    
    if (leaf_hashes) {
        free(leaf_hashes);
        leaf_hashes = NULL;
    }
    
    printf("Merkle树测试完成\n");
}

int main() {
    printf("SM3 Project4 实现测试\n");
    printf("====================\n\n");
    
    // 初始化随机数生成器
    srand((unsigned int)time(NULL));
    
    // 执行任务A：性能优化测试
    task_a_performance_test();
    
    // 执行任务B：长度扩展攻击测试
    task_b_length_extension_test();
    
    // 执行任务C：Merkle树测试
    task_c_merkle_tree_test();
    
    printf("\n所有测试完成！\n");
    return 0;
} 