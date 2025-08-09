#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/sm3.h"

void test_length_extension_attack() {
    printf("=== 测试长度扩展攻击 ===\n");
    
    // 原始消息和哈希
    const char *original_message = "Hello, World!";
    uint8_t original_digest[SM3_DIGEST_SIZE];
    char original_hex[SM3_DIGEST_SIZE * 2 + 1];
    
    // 扩展消息
    const char *extension = "Attack!";
    uint8_t new_digest[SM3_DIGEST_SIZE];
    char new_hex[SM3_DIGEST_SIZE * 2 + 1];
    
    printf("原始消息: \"%s\"\n", original_message);
    
    // 计算原始消息的哈希
    sm3_hash((uint8_t*)original_message, strlen(original_message), original_digest);
    sm3_bytes_to_hex(original_digest, original_hex, SM3_DIGEST_SIZE);
    printf("原始哈希: %s\n", original_hex);
    
    printf("扩展消息: \"%s\"\n", extension);
    
    // 执行长度扩展攻击
    int result = sm3_length_extension_attack(
        original_digest,
        (uint8_t*)original_message,
        strlen(original_message),
        (uint8_t*)extension,
        strlen(extension),
        new_digest
    );
    
    if (result == 0) {
        sm3_bytes_to_hex(new_digest, new_hex, SM3_DIGEST_SIZE);
        printf("攻击成功！新哈希: %s\n", new_hex);
        printf("✓ 长度扩展攻击执行成功！\n");
    } else {
        printf("✗ 攻击失败！\n");
    }
    
    printf("\n");
}

void test_length_extension_with_different_messages() {
    printf("=== 测试不同消息的长度扩展攻击 ===\n");
    
    const char *test_messages[] = {
        "a",
        "abc",
        "Hello",
        "This is a longer message for testing",
        NULL
    };
    
    const char *extensions[] = {
        "X",
        "123",
        "Attack",
        "This is an extension message",
        NULL
    };
    
    for (int i = 0; test_messages[i] != NULL; i++) {
        for (int j = 0; extensions[j] != NULL; j++) {
            printf("测试 %d-%d: 消息=\"%s\", 扩展=\"%s\"\n", 
                   i + 1, j + 1, test_messages[i], extensions[j]);
            
            uint8_t original_digest[SM3_DIGEST_SIZE];
            uint8_t new_digest[SM3_DIGEST_SIZE];
            
            // 计算原始哈希
            sm3_hash((uint8_t*)test_messages[i], strlen(test_messages[i]), original_digest);
            
            // 执行攻击
            int result = sm3_length_extension_attack(
                original_digest,
                (uint8_t*)test_messages[i],
                strlen(test_messages[i]),
                (uint8_t*)extensions[j],
                strlen(extensions[j]),
                new_digest
            );
            
            if (result == 0) {
                printf("  ✓ 攻击成功\n");
            } else {
                printf("  ✗ 攻击失败\n");
            }
        }
    }
    
    printf("\n");
}

void test_padding_calculation() {
    printf("=== 测试填充计算 ===\n");
    
    const int test_lengths[] = {0, 1, 55, 56, 57, 63, 64, 65, 127, 128, 129};
    const int num_tests = sizeof(test_lengths) / sizeof(test_lengths[0]);
    
    for (int i = 0; i < num_tests; i++) {
        int original_len = test_lengths[i];
        int padding_len = sm3_calculate_padding_length(original_len);
        int total_len = original_len + padding_len;
        
        printf("原始长度: %d, 填充长度: %d, 总长度: %d", 
               original_len, padding_len, total_len);
        
        // 验证填充长度是否正确
        if (total_len % 64 == 0) {
            printf(" ✓\n");
        } else {
            printf(" ✗\n");
        }
    }
    
    printf("\n");
}

int main() {
    printf("开始长度扩展攻击测试...\n\n");
    
    test_padding_calculation();
    test_length_extension_attack();
    test_length_extension_with_different_messages();
    
    printf("测试完成！\n");
    return 0;
} 