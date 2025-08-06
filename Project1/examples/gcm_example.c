#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/sm4_gcm.h"

// 测试密钥
static const uint8_t test_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

// 测试IV
static uint8_t test_iv[12];

// 测试AAD
static const uint8_t test_aad[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

// 测试明文
static const uint8_t test_plaintext[] = "Hello, SM4-GCM!";

// 打印十六进制数据
void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// 测试基本GCM加密解密
void test_basic_gcm() {
    printf("=== 基本GCM加密解密测试 ===\n");
    
    size_t plaintext_len = strlen((char*)test_plaintext);
    uint8_t ciphertext[sizeof(test_plaintext)];
    uint8_t decrypted[sizeof(test_plaintext)];
    uint8_t tag[16];
    
    print_hex("密钥", test_key, 16);
    print_hex("IV", test_iv, 12);
    print_hex("AAD", test_aad, 16);
    print_hex("明文", test_plaintext, plaintext_len);
    printf("\n");
    
    // GCM加密
    int result = sm4_gcm_encrypt(test_key, test_iv, 12, 
                                 test_plaintext, plaintext_len,
                                 test_aad, 16,
                                 ciphertext, tag);
    
    if (result == 0) {
        printf("✓ GCM加密成功\n");
        print_hex("密文", ciphertext, plaintext_len);
        print_hex("认证标签", tag, 16);
        printf("\n");
        
        // GCM解密
        result = sm4_gcm_decrypt(test_key, test_iv, 12,
                                ciphertext, plaintext_len,
                                test_aad, 16,
                                tag, decrypted);
        
        if (result == 0) {
            printf("✓ GCM解密成功\n");
            print_hex("解密结果", decrypted, plaintext_len);
            
            // 验证解密结果
            if (memcmp(test_plaintext, decrypted, plaintext_len) == 0) {
                printf("✓ 解密结果正确\n");
            } else {
                printf("✗ 解密结果错误\n");
            }
            
            // 认证标签已在解密函数中验证
            printf("✓ 认证标签验证通过\n");
        } else {
            printf("✗ GCM解密失败\n");
        }
    } else {
        printf("✗ GCM加密失败\n");
    }
    printf("\n");
}

// 测试不同长度的数据
void test_variable_length() {
    printf("=== 变长数据GCM测试 ===\n");
    
    const char *test_strings[] = {
        "Hello, SM4-GCM!",
        "这是一个测试字符串，包含中文字符。",
        "Short",
        "Very long string with many characters to test the GCM mode with different data lengths and see how it performs with various input sizes."
    };
    
    for (int i = 0; i < 4; i++) {
        const char *test_str = test_strings[i];
        size_t len = strlen(test_str);
        
        printf("测试 %d: 长度 %zu 字节\n", i + 1, len);
        printf("数据: %s\n", test_str);
        
        uint8_t ciphertext[1024];
        uint8_t decrypted[1024];
        uint8_t tag[16];
        
        // 加密
        int result = sm4_gcm_encrypt(test_key, test_iv, 12,
                                    (const uint8_t*)test_str, len,
                                    test_aad, 16,
                                    ciphertext, tag);
        
        if (result == 0) {
            printf("✓ 加密成功\n");
            
            // 解密
            result = sm4_gcm_decrypt(test_key, test_iv, 12,
                                    ciphertext, len,
                                    test_aad, 16,
                                    tag, decrypted);
            
            if (result == 0) {
                decrypted[len] = '\0'; // 确保字符串结束
                printf("✓ 解密成功\n");
                printf("解密结果: %s\n", decrypted);
                
                if (strcmp(test_str, (char*)decrypted) == 0) {
                    printf("✓ 数据完整性验证通过\n");
                } else {
                    printf("✗ 数据完整性验证失败\n");
                }
            } else {
                printf("✗ 解密失败\n");
            }
        } else {
            printf("✗ 加密失败\n");
        }
        printf("\n");
    }
}

// 测试认证失败的情况
void test_authentication_failure() {
    printf("=== 认证失败测试 ===\n");
    
    size_t plaintext_len = strlen((char*)test_plaintext);
    uint8_t ciphertext[512];
    uint8_t decrypted[512];
    uint8_t tag[16];
    
    // 正常加密
    int result = sm4_gcm_encrypt(test_key, test_iv, 12,
                                test_plaintext, plaintext_len,
                                test_aad, 16,
                                ciphertext, tag);
    
    if (result == 0) {
        printf("✓ 正常加密成功\n");
        
        // 篡改密文
        ciphertext[0] ^= 0x01;
        printf("⚠ 篡改密文第一个字节\n");
        
        // 尝试解密
        result = sm4_gcm_decrypt(test_key, test_iv, 12,
                                ciphertext, plaintext_len,
                                test_aad, 16,
                                tag, decrypted);
        
        if (result != 0) {
            printf("✓ 认证失败检测正确\n");
        } else {
            printf("✗ 认证失败检测错误\n");
        }
        
        // 恢复密文，篡改标签
        ciphertext[0] ^= 0x01; // 恢复
        tag[0] ^= 0x01; // 篡改标签
        
        printf("⚠ 篡改认证标签第一个字节\n");
        
        result = sm4_gcm_decrypt(test_key, test_iv, 12,
                                ciphertext, plaintext_len,
                                test_aad, 16,
                                tag, decrypted);
        
        if (result != 0) {
            printf("✓ 标签认证失败检测正确\n");
        } else {
            printf("✗ 标签认证失败检测错误\n");
        }
    } else {
        printf("✗ 加密失败\n");
    }
    printf("\n");
}

// 测试性能
void test_gcm_performance() {
    printf("=== GCM性能测试 ===\n");
    
    const size_t data_size = 1024 * 1024; // 1MB
    uint8_t *large_data = malloc(data_size);
    uint8_t *ciphertext = malloc(data_size);
    uint8_t *decrypted = malloc(data_size);
    uint8_t tag[16];
    
    // 初始化大数据
    for (size_t i = 0; i < data_size; i++) {
        large_data[i] = (uint8_t)(i & 0xFF);
    }
    
    printf("测试数据大小: %zu 字节 (%.2f MB)\n", data_size, data_size / (1024.0 * 1024.0));
    
    // 加密性能测试
    clock_t start = clock();
    int result = sm4_gcm_encrypt(test_key, test_iv, 12,
                                large_data, data_size,
                                test_aad, 16,
                                ciphertext, tag);
    clock_t end = clock();
    
    if (result == 0) {
        double encrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("✓ 加密成功: %.6f秒, %.2f MB/s\n", 
               encrypt_time, data_size / (encrypt_time * 1024 * 1024));
        
        // 解密性能测试
        start = clock();
        result = sm4_gcm_decrypt(test_key, test_iv, 12,
                                ciphertext, data_size,
                                test_aad, 16,
                                tag, decrypted);
        end = clock();
        
        if (result == 0) {
            double decrypt_time = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("✓ 解密成功: %.6f秒, %.2f MB/s\n", 
                   decrypt_time, data_size / (decrypt_time * 1024 * 1024));
            
            // 验证数据完整性
            if (memcmp(large_data, decrypted, data_size) == 0) {
                printf("✓ 大数据完整性验证通过\n");
            } else {
                printf("✗ 大数据完整性验证失败\n");
            }
        } else {
            printf("✗ 解密失败\n");
        }
    } else {
        printf("✗ 加密失败\n");
    }
    
    free(large_data);
    free(ciphertext);
    free(decrypted);
    printf("\n");
}

int main() {
    printf("SM4-GCM模式使用示例\n");
    printf("==================\n\n");
    
    // 生成随机IV
    sm4_gcm_generate_iv(test_iv);
    
    // 运行各种测试
    test_basic_gcm();
    test_variable_length();
    test_authentication_failure();
    test_gcm_performance();
    
    printf("GCM模式测试完成！\n");
    return 0;
} 