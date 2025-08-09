#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/sm3.h"

/**
 * Merkle树示例程序
 * 演示如何使用SM3哈希函数构建和验证Merkle树
 */

void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void example_basic_merkle_tree() {
    printf("=== 基本Merkle树示例 ===\n");
    
    // 创建测试数据
    const int leaf_count = 8;
    uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
    
    if (!leaf_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    // 生成叶子哈希
    printf("生成叶子哈希...\n");
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
    printf("\n创建Merkle树...\n");
    merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
    if (!tree) {
        printf("Merkle树创建失败！\n");
        return;
    }
    
    printf("Merkle树创建成功！\n");
    printf("叶子节点数量: %d\n", tree->leaf_count);
    printf("树高度: %d\n", tree->height);
    
    char root_hex[SM3_DIGEST_SIZE * 2 + 1];
    uint8_t *root_hash = merkle_tree_get_root_hash(tree);
    sm3_bytes_to_hex(root_hash, root_hex, SM3_DIGEST_SIZE);
    printf("根哈希: %s\n", root_hex);
    
    // 测试存在性证明
    printf("\n测试存在性证明...\n");
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
}

void example_certificate_transparency() {
    printf("\n=== 证书透明度示例 ===\n");
    
    // 模拟证书透明度日志
    const int cert_count = 10;
    uint8_t **cert_hashes = malloc(cert_count * sizeof(uint8_t*));
    
    if (!cert_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    printf("生成证书哈希...\n");
    for (int i = 0; i < cert_count; i++) {
        cert_hashes[i] = malloc(SM3_DIGEST_SIZE);
        if (!cert_hashes[i]) {
            printf("证书哈希内存分配失败！\n");
            return;
        }
        
        // 模拟证书数据
        char cert_data[128];
        snprintf(cert_data, sizeof(cert_data), 
                "Certificate for domain%d.example.com, issued by CA%d, valid until 2024", 
                i, i % 3);
        
        sm3_hash((uint8_t*)cert_data, strlen(cert_data), cert_hashes[i]);
        
        char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
        sm3_bytes_to_hex(cert_hashes[i], hex_digest, SM3_DIGEST_SIZE);
        printf("证书 %d: %s\n", i, hex_digest);
    }
    
    // 创建证书透明度Merkle树
    printf("\n创建证书透明度Merkle树...\n");
    merkle_tree_t *ct_tree = merkle_tree_create(cert_hashes, cert_count);
    if (!ct_tree) {
        printf("证书透明度树创建失败！\n");
        return;
    }
    
    char root_hex[SM3_DIGEST_SIZE * 2 + 1];
    uint8_t *root_hash = merkle_tree_get_root_hash(ct_tree);
    sm3_bytes_to_hex(root_hash, root_hex, SM3_DIGEST_SIZE);
    printf("证书透明度根哈希: %s\n", root_hex);
    
    // 模拟审计路径
    printf("\n生成审计路径...\n");
    for (int i = 0; i < cert_count; i++) {
        merkle_proof_t *audit_path = merkle_tree_create_existence_proof(ct_tree, i);
        if (audit_path) {
            printf("证书 %d 的审计路径长度: %d\n", i, audit_path->step_count);
            
            // 验证审计路径
            int valid = merkle_tree_verify_existence_proof(ct_tree, audit_path);
            if (valid) {
                printf("  ✓ 审计路径验证成功\n");
            } else {
                printf("  ✗ 审计路径验证失败\n");
            }
            
            merkle_proof_destroy(audit_path);
        }
    }
    
    // 清理
    merkle_tree_destroy(ct_tree);
    for (int i = 0; i < cert_count; i++) {
        free(cert_hashes[i]);
    }
    free(cert_hashes);
}

void example_large_merkle_tree() {
    printf("\n=== 大型Merkle树示例 ===\n");
    
    const int leaf_count = 1000; // 1000个叶子节点
    printf("创建包含 %d 个叶子节点的大型Merkle树...\n", leaf_count);
    
    uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
    if (!leaf_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    // 生成叶子哈希
    printf("生成叶子哈希...\n");
    for (int i = 0; i < leaf_count; i++) {
        leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
        if (!leaf_hashes[i]) {
            printf("叶子哈希内存分配失败！\n");
            return;
        }
        
        char message[64];
        snprintf(message, sizeof(message), "large_tree_leaf_%d_data_for_testing", i);
        sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        
        if (i % 100 == 0) {
            printf("已生成 %d 个叶子哈希...\n", i);
        }
    }
    
    // 创建Merkle树
    printf("构建Merkle树...\n");
    clock_t start = clock();
    merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
    clock_t end = clock();
    
    if (!tree) {
        printf("大型Merkle树创建失败！\n");
        return;
    }
    
    double create_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("树创建完成，耗时: %.2f 毫秒\n", create_time);
    
    char root_hex[SM3_DIGEST_SIZE * 2 + 1];
    uint8_t *root_hash = merkle_tree_get_root_hash(tree);
    sm3_bytes_to_hex(root_hash, root_hex, SM3_DIGEST_SIZE);
    printf("根哈希: %s\n", root_hex);
    
    // 测试随机叶子节点的证明
    printf("\n测试随机叶子节点的存在性证明...\n");
    const int test_count = 10;
    for (int i = 0; i < test_count; i++) {
        int leaf_index = rand() % leaf_count;
        
        start = clock();
        merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, leaf_index);
        end = clock();
        double proof_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        
        if (proof) {
            printf("叶子 %d 的证明创建时间: %.2f 毫秒\n", leaf_index, proof_time);
            
            start = clock();
            int valid = merkle_tree_verify_existence_proof(tree, proof);
            end = clock();
            double verify_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
            
            printf("  验证时间: %.2f 毫秒\n", verify_time);
            printf("  验证结果: %s\n", valid ? "成功" : "失败");
            
            merkle_proof_destroy(proof);
        }
    }
    
    // 清理
    merkle_tree_destroy(tree);
    for (int i = 0; i < leaf_count; i++) {
        free(leaf_hashes[i]);
    }
    free(leaf_hashes);
}

void example_nonexistence_proof() {
    printf("\n=== 不存在性证明示例 ===\n");
    
    const int leaf_count = 16;
    uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
    
    if (!leaf_hashes) {
        printf("内存分配失败！\n");
        return;
    }
    
    // 生成叶子哈希
    for (int i = 0; i < leaf_count; i++) {
        leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
        char message[32];
        snprintf(message, sizeof(message), "leaf_%d", i);
        sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
    }
    
    // 创建Merkle树
    merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
    if (!tree) {
        printf("Merkle树创建失败！\n");
        return;
    }
    
    // 创建一个不存在的元素的哈希
    uint8_t non_existent_hash[SM3_DIGEST_SIZE];
    sm3_hash((uint8_t*)"non_existent_leaf", strlen("non_existent_leaf"), non_existent_hash);
    
    // 测试不存在性证明
    const int test_positions[] = {-1, 20};  // 只测试确实不存在的位置
    const int num_tests = sizeof(test_positions) / sizeof(test_positions[0]);
    
    for (int i = 0; i < num_tests; i++) {
        int position = test_positions[i];
        printf("测试位置 %d 的不存在性证明...\n", position);
        
        merkle_proof_t *proof = merkle_tree_create_nonexistence_proof(tree, position, non_existent_hash);
        if (proof) {
            int valid = merkle_tree_verify_nonexistence_proof(tree, proof);
            printf("  验证结果: %s\n", valid ? "成功" : "失败");
            merkle_proof_destroy(proof);
        } else {
            printf("  证明创建失败\n");
        }
    }
    
    // 清理
    merkle_tree_destroy(tree);
    for (int i = 0; i < leaf_count; i++) {
        free(leaf_hashes[i]);
    }
    free(leaf_hashes);
}

void example_performance_benchmark() {
    printf("\n=== 性能基准测试 ===\n");
    
    const int test_sizes[] = {100, 1000, 10000};
    const int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int leaf_count = test_sizes[s];
        printf("\n测试 %d 个叶子节点...\n", leaf_count);
        
        uint8_t **leaf_hashes = malloc(leaf_count * sizeof(uint8_t*));
        for (int i = 0; i < leaf_count; i++) {
            leaf_hashes[i] = malloc(SM3_DIGEST_SIZE);
            char message[64];
            snprintf(message, sizeof(message), "perf_test_leaf_%d", i);
            sm3_hash((uint8_t*)message, strlen(message), leaf_hashes[i]);
        }
        
        // 测量树创建时间
        clock_t start = clock();
        merkle_tree_t *tree = merkle_tree_create(leaf_hashes, leaf_count);
        clock_t end = clock();
        double create_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        
        if (tree) {
            printf("  树创建时间: %.2f 毫秒\n", create_time);
            
            // 测量证明创建时间
            start = clock();
            merkle_proof_t *proof = merkle_tree_create_existence_proof(tree, 0);
            end = clock();
            double proof_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
            
            if (proof) {
                printf("  证明创建时间: %.2f 毫秒\n", proof_time);
                
                // 测量验证时间
                start = clock();
                int valid = merkle_tree_verify_existence_proof(tree, proof);
                end = clock();
                double verify_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
                
                printf("  验证时间: %.2f 毫秒\n", verify_time);
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
    }
}

void example_error_handling() {
    printf("\n=== 错误处理示例 ===\n");
    
    // 测试空输入
    printf("测试空输入...\n");
    merkle_tree_t *empty_tree = merkle_tree_create(NULL, 0);
    if (!empty_tree) {
        printf("  ✓ 空树创建失败（正确处理）\n");
    } else {
        printf("  ✗ 空树创建成功（错误）\n");
        merkle_tree_destroy(empty_tree);
    }
    
    // 测试无效输入
    printf("测试无效输入...\n");
    merkle_tree_t *invalid_tree = merkle_tree_create(NULL, 10);
    if (!invalid_tree) {
        printf("  ✓ 无效输入处理成功\n");
    } else {
        printf("  ✗ 无效输入处理失败\n");
        merkle_tree_destroy(invalid_tree);
    }
    
    // 测试边界情况
    printf("测试边界情况...\n");
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
}

int main() {
    printf("SM3 Merkle树示例程序\n");
    printf("====================\n\n");
    
    // 设置随机种子
    srand(time(NULL));
    
    example_basic_merkle_tree();
    example_certificate_transparency();
    example_large_merkle_tree();
    example_nonexistence_proof();
    example_performance_benchmark();
    example_error_handling();
    
    printf("\n示例程序执行完成！\n");
    return 0;
}