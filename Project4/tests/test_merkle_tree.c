#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "../include/sm3.h"

void test_merkle_tree_basic() {
    printf("=== 测试Merkle树基本功能 ===\n");
    
    // 创建测试数据
    const int leaf_count = 8;
    uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
    
    if (!leaf_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    // 生成叶子哈希
    for (int i = 0; i < leaf_count; i++) {
        leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
        if (!leaf_hashes[i]) {
            printf("叶子哈希内存分配失败！\n");
            return;
        }
        
        char message[32];
        snprintf(message, sizeof(message), "leaf_%d", i);
        sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        
        char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
        sm3_bytes_to_hex(leaf_hashes[i], hex_digest, SM3_DIGEST_SIZE);
        printf("叶子 %d: %s\n", i, hex_digest);
    }
    
    // 创建Merkle树
    merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
    if (!tree) {
        printf("Merkle树创建失败！\n");
        return;
    }
    
    printf("Merkle树创建成功，根哈希: ");
    char root_hex[SM3_DIGEST_SIZE * 2 + 1];
    uint8_t *root_hash = merkle_tree_get_root_hash(tree);
    sm3_bytes_to_hex(root_hash, root_hex, SM3_DIGEST_SIZE);
    printf("%s\n", root_hex);
    
    // 测试存在性证明
    for (int i = 0; i < leaf_count; i++) {
        printf("测试叶子 %d 的存在性证明...\n", i);
        
        merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, i);
        if (!proof) {
            printf("  ✗ 证明创建失败\n");
            continue;
        }
        
        int valid = merkle_tree_verify_existence_proof(tree, proof);
        if (valid) {
            printf("  ✓ 存在性证明验证成功\n");
        } else {
            printf("  ✗ 存在性证明验证失败\n");
        }
        
        merkle_proof_destroy(proof);
    }
    
    // 清理
    merkle_tree_destroy(tree);
    for (int i = 0; i < leaf_count; i++) {
        free(leaf_hashes[i]);
    }
    free(leaf_hashes);
    
    printf("\n");
}

void test_merkle_tree_large() {
    printf("=== 测试大型Merkle树 ===\n");
    
    const int leaf_count = 1000; // 1000个叶子节点
    uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
    
    if (!leaf_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    printf("生成 %d 个叶子哈希...\n", leaf_count);
    
    // 生成叶子哈希
    for (int i = 0; i < leaf_count; i++) {
        leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
        if (!leaf_hashes[i]) {
            printf("叶子哈希内存分配失败！\n");
            return;
        }
        
        char message[64];
        snprintf(message, sizeof(message), "large_leaf_%d_data_for_testing", i);
        sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        
        if (i % 100 == 0) {
            printf("已生成 %d 个叶子哈希...\n", i);
        }
    }
    
    printf("创建Merkle树...\n");
    merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
    if (!tree) {
        printf("Merkle树创建失败！\n");
        return;
    }
    
    printf("Merkle树创建成功！\n");
    char root_hex[SM3_DIGEST_SIZE * 2 + 1];
    uint8_t *root_hash = merkle_tree_get_root_hash(tree);
    sm3_bytes_to_hex(root_hash, root_hex, SM3_DIGEST_SIZE);
    printf("根哈希: %s\n", root_hex);
    
    // 测试随机叶子节点的存在性证明
    const int test_count = 10;
    for (int i = 0; i < test_count; i++) {
        int leaf_index = rand() % leaf_count;
        printf("测试叶子 %d 的存在性证明...\n", leaf_index);
        
        merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, leaf_index);
        if (!proof) {
            printf("  ✗ 证明创建失败\n");
            continue;
        }
        
        int valid = merkle_tree_verify_existence_proof(tree, proof);
        if (valid) {
            printf("  ✓ 存在性证明验证成功\n");
        } else {
            printf("  ✗ 存在性证明验证失败\n");
        }
        
        merkle_proof_destroy(proof);
    }
    
    // 清理
    merkle_tree_destroy(tree);
    for (int i = 0; i < leaf_count; i++) {
        free(leaf_hashes[i]);
    }
    free(leaf_hashes);
    
    printf("\n");
}

void test_merkle_tree_edge_cases() {
    printf("=== 测试Merkle树边界情况 ===\n");
    
    // 测试单个叶子节点
    printf("测试单个叶子节点...\n");
    uint8_t *single_leaf = malloc(SM3_DIGEST_SIZE);
    sm3_hash((uint8_t*)"single", 6, single_leaf);
    
    uint8_t **single_leaf_array = malloc(sizeof(uint8_t*));
    single_leaf_array[0] = single_leaf;
    
    merkle_tree_t *single_tree = merkle_tree_create(single_leaf_array, 1);
    if (single_tree) {
        printf("  ✓ 单个叶子节点树创建成功\n");
        merkle_tree_destroy(single_tree);
    } else {
        printf("  ✗ 单个叶子节点树创建失败\n");
    }
    
    free(single_leaf_array);
    free(single_leaf);
    
    // 测试空树
    printf("测试空树...\n");
    merkle_tree_t *empty_tree = merkle_tree_create(NULL, 0);
    if (empty_tree) {
        printf("  ✓ 空树创建成功\n");
        merkle_tree_destroy(empty_tree);
    } else {
        printf("  ✗ 空树创建失败\n");
    }
    
    printf("\n");
}

void test_merkle_tree_performance() {
    printf("=== 测试Merkle树性能 ===\n");
    
    const int test_sizes[] = {10, 100, 1000, 10000};
    const int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int t = 0; t < num_tests; t++) {
        int leaf_count = test_sizes[t];
        printf("测试 %d 个叶子节点...\n", leaf_count);
        
        uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
        if (!leaf_hashes) {
            printf("内存分配失败！\n");
            continue;
        }
        
        // 生成叶子哈希
        for (int i = 0; i < leaf_count; i++) {
            leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
            char message[64];
            snprintf(message, sizeof(message), "perf_test_leaf_%d", i);
            sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        }
        
        // 测量创建时间
        clock_t start = clock();
        merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
        clock_t end = clock();
        
        if (tree) {
            double create_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
            printf("  创建时间: %.2f ms\n", create_time);
            
            // 测量证明创建时间
            start = clock();
            merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, 0);
            end = clock();
            
            if (proof) {
                double proof_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
                printf("  证明创建时间: %.2f ms\n", proof_time);
                
                // 测量验证时间
                start = clock();
                int valid = merkle_tree_verify_existence_proof(tree, proof);
                end = clock();
                
                double verify_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
                printf("  验证时间: %.2f ms\n", verify_time);
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

void test_merkle_tree_100k_leaves() {
    printf("=== 测试10万叶子节点的Merkle树 (RFC6962) ===\n");
    
    const int leaf_count = 100000; // 10万叶子节点
    printf("创建包含 %d 个叶子节点的Merkle树...\n", leaf_count);
    
    // 分配内存
    uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
    if (!leaf_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    printf("生成叶子哈希...\n");
    clock_t start_time = clock();
    
    // 生成叶子哈希
    for (int i = 0; i < leaf_count; i++) {
        leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
        if (!leaf_hashes[i]) {
            printf("叶子哈希内存分配失败！\n");
            return;
        }
        
        char message[32];
        snprintf(message, sizeof(message), "leaf_%d", i);
        sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        
        // 每10000个显示一次进度
        if ((i + 1) % 10000 == 0) {
            printf("已生成 %d 个叶子哈希...\n", i + 1);
        }
    }
    
    clock_t hash_time = clock();
    printf("叶子哈希生成完成，耗时: %.2f 秒\n", 
           (double)(hash_time - start_time) / CLOCKS_PER_SEC);
    
    // 创建Merkle树
    printf("构建Merkle树...\n");
    start_time = clock();
    merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
    clock_t tree_time = clock();
    
    if (!tree) {
        printf("Merkle树创建失败！\n");
        return;
    }
    
    printf("Merkle树构建完成，耗时: %.2f 秒\n", 
           (double)(tree_time - start_time) / CLOCKS_PER_SEC);
    
    // 显示根哈希
    char root_hex[SM3_DIGEST_SIZE * 2 + 1];
    uint8_t *root_hash = merkle_tree_get_root_hash(tree);
    sm3_bytes_to_hex(root_hash, root_hex, SM3_DIGEST_SIZE);
    printf("根哈希: %s\n", root_hex);
    
    // 测试存在性证明
    printf("\n=== 测试存在性证明 ===\n");
    const int test_indices[] = {0, 1000, 50000, 99999}; // 测试不同位置的叶子
    
    for (int i = 0; i < sizeof(test_indices) / sizeof(test_indices[0]); i++) {
        int leaf_index = test_indices[i];
        printf("测试叶子 %d 的存在性证明...\n", leaf_index);
        
        clock_t proof_start = clock();
        merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, leaf_index);
        clock_t proof_end = clock();
        
        if (!proof) {
            printf("  ✗ 证明创建失败\n");
            continue;
        }
        
        printf("  证明创建耗时: %.3f 毫秒\n", 
               (double)(proof_end - proof_start) * 1000 / CLOCKS_PER_SEC);
        
        clock_t verify_start = clock();
        int valid = merkle_tree_verify_existence_proof(tree, proof);
        clock_t verify_end = clock();
        
        if (valid) {
            printf("  ✓ 存在性证明验证成功\n");
        } else {
            printf("  ✗ 存在性证明验证失败\n");
        }
        
        printf("  验证耗时: %.3f 毫秒\n", 
               (double)(verify_end - verify_start) * 1000 / CLOCKS_PER_SEC);
        
        merkle_proof_destroy(proof);
    }
    
    // 测试不存在性证明
    printf("\n=== 测试不存在性证明 ===\n");
    
    // 创建一个不在树中的哈希
    uint8_t non_existent_hash[SM3_DIGEST_SIZE];
    char non_existent_message[] = "this_hash_is_not_in_the_tree";
    sm3_hash((uint8_t*)non_existent_message, strlen(non_existent_message), non_existent_hash);
    
    // 测试在树中间位置插入不存在的哈希
    int insert_position = 50000;
    printf("测试在位置 %d 插入不存在哈希的不存在性证明...\n", insert_position);
    
    clock_t non_proof_start = clock();
    merkle_proof_t *non_proof = merkle_tree_create_nonexistence_proof(tree, insert_position, non_existent_hash);
    clock_t non_proof_end = clock();
    
    if (!non_proof) {
        printf("  ✗ 不存在性证明创建失败\n");
    } else {
        printf("  证明创建耗时: %.3f 毫秒\n", 
               (double)(non_proof_end - non_proof_start) * 1000 / CLOCKS_PER_SEC);
        
        clock_t non_verify_start = clock();
        int non_valid = merkle_tree_verify_nonexistence_proof(tree, non_proof);
        clock_t non_verify_end = clock();
        
        if (non_valid) {
            printf("  ✓ 不存在性证明验证成功\n");
        } else {
            printf("  ✗ 不存在性证明验证失败\n");
        }
        
        printf("  验证耗时: %.3f 毫秒\n", 
               (double)(non_verify_end - non_verify_start) * 1000 / CLOCKS_PER_SEC);
        
        merkle_proof_destroy(non_proof);
    }
    
    // 清理
    printf("\n清理内存...\n");
    merkle_tree_destroy(tree);
    for (int i = 0; i < leaf_count; i++) {
        free(leaf_hashes[i]);
    }
    free(leaf_hashes);
    
    printf("10万叶子节点Merkle树测试完成！\n\n");
}

int main() {
    printf("开始Merkle树测试...\n\n");
    
    test_merkle_tree_basic();
    test_merkle_tree_edge_cases();
    test_merkle_tree_large();
    test_merkle_tree_performance();
    test_merkle_tree_100k_leaves();
    
    printf("测试完成！\n");
    return 0;
} 