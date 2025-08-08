#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/sm3.h"

// 测试向量
typedef struct {
    const char *message;
    const char *expected_hash;
} test_vector_t;

// 正确的SM3测试向量（使用OpenSSL验证）
static test_vector_t test_vectors[] = {
    {"", "1ab21d8355cfa17f8e61194831e81a8f22bec8c728fefb747ed035eb5082aa2b"},
    {"a", "623476ac18f65a2909e43c7fec61b49c7e764a91a18ccb82f1917a29c86c5e88"},
    {"abc", "66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0"},
    {"abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd", 
     "debe9ff92275b8a138604889c18e5a4d6fdb70e5387e5765293dcba39c0c5732"},
    {"abcdefghijklmnopqrstuvwxyz", 
     "b80fe97a4da24afc277564f66a359ef440462ad28dcc6d63adb24d5c20a61595"},
    {NULL, NULL}
};

void test_sm3_basic() {
    printf("=== 测试SM3基本实现 ===\n");
    
    uint8_t digest[SM3_DIGEST_SIZE];
    char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
    
    for (int i = 0; test_vectors[i].message != NULL; i++) {
        printf("测试 %d: 消息 = \"%s\"\n", i + 1, test_vectors[i].message);
        
        // 计算哈希
        sm3_hash((uint8_t*)test_vectors[i].message, 
                 strlen(test_vectors[i].message), 
                 digest);
        
        // 转换为十六进制
        sm3_bytes_to_hex(digest, hex_digest, SM3_DIGEST_SIZE);
        
        printf("期望: %s\n", test_vectors[i].expected_hash);
        printf("实际: %s\n", hex_digest);
        
        if (strcmp(hex_digest, test_vectors[i].expected_hash) == 0) {
            printf("✓ 通过\n");
        } else {
            printf("✗ 失败\n");
        }
        printf("\n");
    }
}

void test_sm3_optimized() {
    printf("=== 测试SM3优化版实现 ===\n");
    
    uint8_t digest[SM3_DIGEST_SIZE];
    char hex_digest[SM3_DIGEST_SIZE * 2 + 1];
    
    for (int i = 0; test_vectors[i].message != NULL; i++) {
        printf("测试 %d: 消息 = \"%s\"\n", i + 1, test_vectors[i].message);
        
        // 计算哈希
        sm3_hash_optimized((uint8_t*)test_vectors[i].message, 
                          strlen(test_vectors[i].message), 
                          digest);
        
        // 转换为十六进制
        sm3_bytes_to_hex(digest, hex_digest, SM3_DIGEST_SIZE);
        
        printf("期望: %s\n", test_vectors[i].expected_hash);
        printf("实际: %s\n", hex_digest);
        
        if (strcmp(hex_digest, test_vectors[i].expected_hash) == 0) {
            printf("✓ 通过\n");
        } else {
            printf("✗ 失败\n");
        }
        printf("\n");
    }
}

int main() {
    printf("开始SM3基本功能测试...\n\n");
    
    test_sm3_basic();
    test_sm3_optimized();
    
    printf("测试完成！\n");
    return 0;
} 