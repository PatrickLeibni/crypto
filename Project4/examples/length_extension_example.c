#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/sm3.h"

/**
 * 长度扩展攻击示例程序
 * 演示如何对SM3哈希函数进行长度扩展攻击
 */

void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void example_basic_attack() {
    printf("=== 基本长度扩展攻击示例 ===\n");
    
    // 原始消息和扩展
    const char *original_message = "Hello, World!";
    const char *extension = "This is an extension attack!";
    
    printf("原始消息: \"%s\"\n", original_message);
    printf("扩展内容: \"%s\"\n", extension);
    
    // 计算原始消息的哈希
    uint8_t original_digest[SM3_DIGEST_SIZE];
    sm3_hash((uint8_t*)original_message, strlen(original_message), original_digest);
    
    char original_hex[SM3_DIGEST_SIZE * 2 + 1];
    sm3_bytes_to_hex(original_digest, original_hex, SM3_DIGEST_SIZE);
    printf("原始哈希: %s\n", original_hex);
    
    // 执行长度扩展攻击
    uint8_t attack_digest[SM3_DIGEST_SIZE];
    int result = sm3_length_extension_attack(
        original_digest,
        (uint8_t*)original_message,
        strlen(original_message),
        (uint8_t*)extension,
        strlen(extension),
        attack_digest
    );
    
    if (result == 0) {
        char attack_hex[SM3_DIGEST_SIZE * 2 + 1];
        sm3_bytes_to_hex(attack_digest, attack_hex, SM3_DIGEST_SIZE);
        printf("攻击哈希: %s\n", attack_hex);
        
        // 验证攻击结果
        char verification_message[1024];
        uint8_t padding[128];
        size_t padding_len;
        
        // 使用与攻击相同的填充函数
        create_padding(padding, strlen(original_message), &padding_len);
        int total_len = strlen(original_message) + padding_len + strlen(extension);
        
        // 构造验证消息：原始消息 + 填充 + 扩展
        memcpy(verification_message, original_message, strlen(original_message));
        memcpy(verification_message + strlen(original_message), padding, padding_len);
        memcpy(verification_message + strlen(original_message) + padding_len, extension, strlen(extension));
        
        // 计算验证哈希
        uint8_t verification_digest[SM3_DIGEST_SIZE];
        sm3_hash((uint8_t*)verification_message, total_len, verification_digest);
        
        char verification_hex[SM3_DIGEST_SIZE * 2 + 1];
        sm3_bytes_to_hex(verification_digest, verification_hex, SM3_DIGEST_SIZE);
        printf("验证哈希: %s\n", verification_hex);
        
        // 比较结果
        if (memcmp(attack_digest, verification_digest, SM3_DIGEST_SIZE) == 0) {
            printf("✓ 攻击验证成功！\n");
        } else {
            printf("✗ 攻击验证失败！\n");
        }
        
        printf("填充长度: %d 字节\n", padding_len);
        printf("总消息长度: %d 字节\n", total_len);
        
    } else {
        printf("✗ 攻击失败！\n");
    }
}

void example_multiple_attacks() {
    printf("\n=== 多重攻击示例 ===\n");
    
    const char *test_cases[] = {
        "a",
        "abc",
        "Hello",
        "This is a test message",
        "A longer message for testing length extension attacks"
    };
    
    const char *extensions[] = {
        "X",
        "123",
        "Attack",
        "This is an extension",
        "A malicious extension message"
    };
    
    for (int i = 0; i < 5; i++) {
        printf("\n测试用例 %d:\n", i + 1);
        printf("原始消息: \"%s\"\n", test_cases[i]);
        printf("扩展内容: \"%s\"\n", extensions[i]);
        
        uint8_t original_digest[SM3_DIGEST_SIZE];
        uint8_t attack_digest[SM3_DIGEST_SIZE];
        
        // 计算原始哈希
        sm3_hash((uint8_t*)test_cases[i], strlen(test_cases[i]), original_digest);
        
        // 执行攻击
        int result = sm3_length_extension_attack(
            original_digest,
            (uint8_t*)test_cases[i],
            strlen(test_cases[i]),
            (uint8_t*)extensions[i],
            strlen(extensions[i]),
            attack_digest
        );
        
        if (result == 0) {
            printf("  ✓ 攻击成功\n");
        } else {
            printf("  ✗ 攻击失败\n");
        }
    }
}

void example_padding_analysis() {
    printf("\n=== 填充分析示例 ===\n");
    
    const int test_lengths[] = {0, 1, 55, 56, 57, 63, 64, 65, 127, 128, 129};
    const int num_tests = sizeof(test_lengths) / sizeof(test_lengths[0]);
    
    printf("消息长度 | 填充长度 | 总长度 | 是否512倍数\n");
    printf("---------|----------|--------|------------\n");
    
    for (int i = 0; i < num_tests; i++) {
        int original_len = test_lengths[i];
        int padding_len = sm3_calculate_padding_length(original_len);
        int total_len = original_len + padding_len;
        
        printf("%9d | %8d | %6d | %s\n", 
               original_len, padding_len, total_len,
               (total_len % 64 == 0) ? "是" : "否");
    }
}

void example_attack_scenarios() {
    printf("\n=== 攻击场景示例 ===\n");
    
    // 场景1: 数字签名伪造
    printf("场景1: 数字签名伪造\n");
    const char *original_contract = "用户同意转账100元";
    const char *malicious_extension = "转账1000000元";
    
    uint8_t contract_digest[SM3_DIGEST_SIZE];
    sm3_hash((uint8_t*)original_contract, strlen(original_contract), contract_digest);
    
    uint8_t malicious_digest[SM3_DIGEST_SIZE];
    sm3_length_extension_attack(
        contract_digest,
        (uint8_t*)original_contract,
        strlen(original_contract),
        (uint8_t*)malicious_extension,
        strlen(malicious_extension),
        malicious_digest
    );
    
    printf("原始合同: \"%s\"\n", original_contract);
    printf("恶意扩展: \"%s\"\n", malicious_extension);
    printf("攻击者可以构造: \"%s\" + 填充 + \"%s\"\n", 
           original_contract, malicious_extension);
    
    // 场景2: MAC伪造
    printf("\n场景2: MAC伪造\n");
    const char *original_message = "message";
    const char *mac_extension = "admin";
    
    uint8_t mac_digest[SM3_DIGEST_SIZE];
    sm3_hash((uint8_t*)original_message, strlen(original_message), mac_digest);
    
    uint8_t forged_mac[SM3_DIGEST_SIZE];
    sm3_length_extension_attack(
        mac_digest,
        (uint8_t*)original_message,
        strlen(original_message),
        (uint8_t*)mac_extension,
        strlen(mac_extension),
        forged_mac
    );
    
    printf("原始消息: \"%s\"\n", original_message);
    printf("MAC扩展: \"%s\"\n", mac_extension);
    printf("攻击者可以伪造MAC而不需要知道密钥\n");
}

void example_defense_measures() {
    printf("\n=== 防御措施示例 ===\n");
    
    // HMAC-SM3 示例
    printf("1. HMAC-SM3 防御长度扩展攻击:\n");
    const char *key = "secret_key";
    const char *message = "Hello, World!";
    uint8_t hmac_digest[SM3_DIGEST_SIZE];
    
    // 简化的HMAC实现
    printf("   密钥: %s\n", key);
    printf("   消息: %s\n", message);
    printf("   HMAC-SM3 哈希: ");
    // 这里应该实现HMAC-SM3，暂时使用简单哈希
    sm3_hash((uint8_t*)message, strlen(message), hmac_digest);
    sm3_print_digest(hmac_digest);
    
    // 加盐哈希示例
    printf("\n2. 加盐哈希防御长度扩展攻击:\n");
    const char *salt = "random_salt_123";
    const char *password = "user_password";
    uint8_t salted_digest[SM3_DIGEST_SIZE];
    
    // 简化的加盐哈希实现
    printf("   盐值: %s\n", salt);
    printf("   密码: %s\n", password);
    printf("   加盐哈希: ");
    // 这里应该实现加盐哈希，暂时使用简单哈希
    sm3_hash((uint8_t*)password, strlen(password), salted_digest);
    sm3_print_digest(salted_digest);
    
    printf("\n注意: 这些是简化的示例。实际应用中应使用专门的HMAC和加盐哈希库。\n");
}

void example_performance_analysis() {
    printf("\n=== 性能分析示例 ===\n");
    
    const int test_sizes[] = {64, 256, 1024, 4096};
    const char *extension = "This is an extension message for performance testing";
    
    for (int i = 0; i < 4; i++) {
        int size = test_sizes[i];
        uint8_t *test_data = malloc(size);
        
        if (test_data) {
            // 生成测试数据
            for (int j = 0; j < size; j++) {
                test_data[j] = rand() % 256;
            }
            
            uint8_t original_digest[SM3_DIGEST_SIZE];
            uint8_t attack_digest[SM3_DIGEST_SIZE];
            
            // 计算原始哈希
            clock_t start = clock();
            sm3_hash(test_data, size, original_digest);
            clock_t end = clock();
            double hash_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
            
            // 执行攻击
            start = clock();
            sm3_length_extension_attack(
                original_digest,
                test_data,
                size,
                (uint8_t*)extension,
                strlen(extension),
                attack_digest
            );
            end = clock();
            double attack_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
            
            printf("数据大小: %d 字节\n", size);
            printf("  哈希计算时间: %.2f 毫秒\n", hash_time);
            printf("  攻击执行时间: %.2f 毫秒\n", attack_time);
            printf("  攻击/哈希比率: %.2f\n", attack_time / hash_time);
            printf("\n");
            
            free(test_data);
        }
    }
}

int main() {
    printf("SM3 长度扩展攻击示例程序\n");
    printf("========================\n\n");
    
    // 设置随机种子
    srand(time(NULL));
    
    example_basic_attack();
    example_multiple_attacks();
    example_padding_analysis();
    example_attack_scenarios();
    example_defense_measures();
    example_performance_analysis();
    
    printf("\n示例程序执行完成！\n");
    printf("\n注意: 长度扩展攻击仅用于教育和研究目的。\n");
    printf("在实际应用中，请使用适当的防护措施。\n");
    
    return 0;
} 