#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 包含所有SM4实现
#include "sm4_basic.h"
#include "sm4_ttable.h"
#include "sm4_aesni.h"
#include "sm4_gfni.h"
#include "sm4_vprold.h"
#include "sm4_avx512_gfni.h"
#include "sm4_avx512_vprold.h"
#include "sm4_gcm.h"



// 测试数据
static const uint8_t test_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

static const uint8_t test_plaintext[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
};

// 打印十六进制数据
void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// 测试基本SM4实现
void test_basic_sm4(void) {
    printf("=== 测试基本SM4实现 ===\n");
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt(test_key, test_plaintext, ciphertext);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt(test_key, ciphertext, decrypted);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("基本SM4实现测试通过\n");
    } else {
        printf("基本SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试T-table优化的SM4实现
void test_ttable_sm4(void) {
    printf("=== 测试T-table优化的SM4实现 ===\n");
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt_ttable(test_key, test_plaintext, ciphertext);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt_ttable(test_key, ciphertext, decrypted);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("T-table优化SM4实现测试通过\n");
    } else {
        printf("T-table优化SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试AESNI优化的SM4实现
void test_aesni_sm4(void) {
    printf("=== 测试AESNI优化的SM4实现 ===\n");
    
    if (!sm4_aesni_available()) {
        printf("AESNI指令集不可用，跳过测试\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt_aesni(test_key, test_plaintext, ciphertext);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt_aesni(test_key, ciphertext, decrypted);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("AESNI优化SM4实现测试通过\n");
    } else {
        printf("AESNI优化SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试GFNI优化的SM4实现
void test_gfni_sm4(void) {
    printf("=== 测试GFNI优化的SM4实现 ===\n");
    
    if (!sm4_gfni_available()) {
        printf("GFNI指令集不可用，跳过测试\n");
        printf("注意：这可能是由于CPU不支持GFNI指令集\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt_gfni(test_key, test_plaintext, ciphertext);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt_gfni(test_key, ciphertext, decrypted);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ GFNI优化SM4实现测试通过\n");
    } else {
        printf("✗ GFNI优化SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试VPROLD优化的SM4实现
void test_vprold_sm4(void) {
    printf("=== 测试VPROLD优化的SM4实现 ===\n");
    
    if (!sm4_vprold_available()) {
        printf("VPROLD指令集不可用，跳过测试\n");
        printf("注意：这可能是由于CPU不支持VPROLD指令集\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt_vprold(test_key, test_plaintext, ciphertext);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt_vprold(test_key, ciphertext, decrypted);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("✓ VPROLD优化SM4实现测试通过\n");
    } else {
        printf("✗ VPROLD优化SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试AVX-512+GFNI优化的SM4实现
void test_avx512_gfni_sm4(void) {
    printf("=== 测试AVX-512和GFNI优化的SM4实现 ===\n");
    
    if (!sm4_avx512_gfni_available()) {
        printf("AVX-512或GFNI指令集不可用，跳过测试\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt_avx512_gfni(test_key, test_plaintext, ciphertext, 1);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt_avx512_gfni(test_key, ciphertext, decrypted, 1);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("AVX-512和GFNI优化SM4实现测试通过\n");
    } else {
        printf("AVX-512和GFNI优化SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试AVX-512+VPROLD优化的SM4实现
void test_avx512_vprold_sm4(void) {
    printf("=== 测试AVX-512和VPROLD优化的SM4实现 ===\n");
    
    if (!sm4_avx512_vprold_available()) {
        printf("AVX-512或VPROLD指令集不可用，跳过测试\n");
        printf("注意：这可能是由于CPU不支持AVX-512+VPROLD指令集\n\n");
        return;
    }
    
    uint8_t ciphertext[16];
    uint8_t decrypted[16];
    
    // 加密
    sm4_encrypt_avx512_vprold(test_key, test_plaintext, ciphertext, 1);
    print_hex("密文", ciphertext, 16);
    
    // 解密
    sm4_decrypt_avx512_vprold(test_key, ciphertext, decrypted, 1);
    print_hex("解密结果", decrypted, 16);
    
    // 验证
    if (memcmp(test_plaintext, decrypted, 16) == 0) {
        printf("AVX-512和VPROLD优化SM4实现测试通过\n");
    } else {
        printf("AVX-512和VPROLD优化SM4实现测试失败\n");
    }
    printf("\n");
}

// 测试SM4-GCM实现
void test_sm4_gcm(void) {
    printf("=== 测试SM4-GCM实现 ===\n");
    
    uint8_t iv[12];
    uint8_t aad[] = "Additional Authenticated Data";
    uint8_t plaintext[] = "Hello, SM4-GCM!";
    size_t plaintext_len = strlen((char*)plaintext);
    size_t aad_len = strlen((char*)aad);
    
    uint8_t ciphertext[512];  // 增大数组大小
    uint8_t tag[16];
    uint8_t decrypted[512];   // 增大数组大小
    
    // 生成IV
    sm4_gcm_generate_iv(iv);
    print_hex("IV", iv, 12);
    print_hex("AAD", aad, aad_len);
    print_hex("明文", plaintext, plaintext_len);
    
    // 测试基础版本加密
    int result = sm4_gcm_encrypt(test_key, iv, 12, plaintext, plaintext_len,
                                 aad, aad_len, ciphertext, tag);
    
    if (result == 0) {
        print_hex("密文", ciphertext, plaintext_len);
        print_hex("认证标签", tag, 16);
        
        // 测试基础版本解密
        result = sm4_gcm_decrypt(test_key, iv, 12, ciphertext, plaintext_len,
                                 aad, aad_len, tag, decrypted);
        
        if (result == 0) {
            print_hex("解密结果", decrypted, plaintext_len);
            // 确保字符串正确终止
            decrypted[plaintext_len] = '\0';
            printf("解密文本: %s\n", decrypted);
            
            if (memcmp(plaintext, decrypted, plaintext_len) == 0) {
                printf("基础SM4-GCM实现测试通过\n");
            } else {
                printf("基础SM4-GCM实现测试失败\n");
            }
        } else {
            printf("基础SM4-GCM解密失败\n");
        }
    } else {
        printf("基础SM4-GCM加密失败\n");
    }
    
    // 测试优化版本
    printf("\n=== 测试优化版SM4-GCM实现 ===\n");
    
    uint8_t ciphertext_opt[512];
    uint8_t tag_opt[16];
    uint8_t decrypted_opt[512];
    
    // 测试优化版本加密
    result = sm4_gcm_encrypt_optimized(test_key, iv, 12, plaintext, plaintext_len,
                                       aad, aad_len, ciphertext_opt, tag_opt);
    
    if (result == 0) {
        print_hex("优化版密文", ciphertext_opt, plaintext_len);
        print_hex("优化版认证标签", tag_opt, 16);
        
        // 测试优化版本解密
        result = sm4_gcm_decrypt_optimized(test_key, iv, 12, ciphertext_opt, plaintext_len,
                                           aad, aad_len, tag_opt, decrypted_opt);
        
        if (result == 0) {
            print_hex("优化版解密结果", decrypted_opt, plaintext_len);
            // 确保字符串正确终止
            decrypted_opt[plaintext_len] = '\0';
            printf("优化版解密文本: %s\n", decrypted_opt);
            
            if (memcmp(plaintext, decrypted_opt, plaintext_len) == 0) {
                printf("优化版SM4-GCM实现测试通过\n");
            } else {
                printf("优化版SM4-GCM实现测试失败\n");
            }
        } else {
            printf("优化版SM4-GCM解密失败\n");
        }
    } else {
        printf("优化版SM4-GCM加密失败\n");
    }
    
    printf("\n");
}

// 性能测试
void performance_test(void) {
    printf("=== SM4优化性能测试 ===\n");
    
    // 显示CPU特性支持情况
    printf("CPU特性检测:\n");
    printf("  AESNI: %s\n", sm4_aesni_available() ? "支持" : "不支持");
    printf("  GFNI: %s\n", sm4_gfni_available() ? "支持" : "不支持");
    printf("  VPROLD: %s\n", sm4_vprold_available() ? "支持" : "不支持");
    printf("  AVX-512+GFNI: %s\n", sm4_avx512_gfni_available() ? "支持" : "不支持");
    printf("  AVX-512+VPROLD: %s\n", sm4_avx512_vprold_available() ? "支持" : "不支持");
    printf("\n");
    
    const int num_blocks = 100000; 
    const size_t data_size = num_blocks * 16;
    
    uint8_t *data = malloc(data_size);
    uint8_t *encrypted = malloc(data_size);
    uint8_t *decrypted = malloc(data_size);
    
    if (!data || !encrypted || !decrypted) {
        printf("内存分配失败\n");
        return;
    }
    
    // 初始化测试数据
    for (size_t i = 0; i < data_size; i++) {
        data[i] = i & 0xFF;
    }
    
    clock_t start, end;
    double cpu_time_used;
    
    // 测试基本实现
    start = clock();
    for (int i = 0; i < num_blocks; i++) {
        sm4_encrypt(test_key, &data[i * 16], &encrypted[i * 16]);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("基本SM4实现: %.3f秒 (%.2f MB/s)\n", 
           cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    
    // 测试T-table优化
    start = clock();
    for (int i = 0; i < num_blocks; i++) {
        sm4_encrypt_ttable(test_key, &data[i * 16], &encrypted[i * 16]);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("T-table优化: %.3f秒 (%.2f MB/s)\n", 
           cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    
    // 测试AESNI优化
    if (sm4_aesni_available()) {
        start = clock();
        for (int i = 0; i < num_blocks; i++) {
            sm4_encrypt_aesni(test_key, &data[i * 16], &encrypted[i * 16]);
        }
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("AESNI优化: %.3f秒 (%.2f MB/s)\n", 
               cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    }
    
    // 测试AVX-512和GFNI优化
    if (sm4_avx512_gfni_available()) {
        start = clock();
        sm4_encrypt_avx512_gfni(test_key, data, encrypted, num_blocks);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("AVX-512+GFNI优化: %.3f秒 (%.2f MB/s)\n", 
               cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    }
    
    // 测试GFNI优化
    if (sm4_gfni_available()) {
        start = clock();
        sm4_encrypt_gfni_batch(test_key, data, encrypted, num_blocks);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("GFNI优化: %.3f秒 (%.2f MB/s)\n", 
               cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    }
    
    // 测试VPROLD优化
    if (sm4_vprold_available()) {
        start = clock();
        sm4_encrypt_vprold_batch(test_key, data, encrypted, num_blocks);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("VPROLD优化: %.3f秒 (%.2f MB/s)\n", 
               cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    }
    
    // 测试AVX-512+VPROLD优化
    if (sm4_avx512_vprold_available()) {
        start = clock();
        sm4_encrypt_avx512_vprold(test_key, data, encrypted, num_blocks);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("AVX-512+VPROLD优化: %.3f秒 (%.2f MB/s)\n", 
               cpu_time_used, (data_size / 1024.0 / 1024.0) / cpu_time_used);
    }
    
    
    // 释放内存
    free(data);
    free(encrypted);
    free(decrypted);
    printf("\n");
}

// SM4-GCM性能测试
void gcm_performance_test(void) {
    printf("=== SM4-GCM 性能测试 ===\n");

    const int gcm_blocks = 60; 
    const size_t gcm_data_size = gcm_blocks * 16;

    uint8_t iv[12];
    uint8_t aad[] = "Additional Authenticated Data for Performance Test";
    size_t aad_len = strlen((char *)aad);

    uint8_t *gcm_data = malloc(gcm_data_size);
    uint8_t *gcm_ciphertext = malloc(gcm_data_size);
    uint8_t *gcm_decrypted = malloc(gcm_data_size);
    uint8_t *gcm_ciphertext_opt = malloc(gcm_data_size);
    uint8_t *gcm_decrypted_opt = malloc(gcm_data_size);
    uint8_t tag[16];
    uint8_t tag_opt[16];
    clock_t start, end;
    double cpu_time_used;

    if (!gcm_data || !gcm_ciphertext || !gcm_decrypted || !gcm_ciphertext_opt || !gcm_decrypted_opt) {
        printf("GCM内存分配失败\n");
        if (gcm_data) free(gcm_data);
        if (gcm_ciphertext) free(gcm_ciphertext);
        if (gcm_decrypted) free(gcm_decrypted);
        if (gcm_ciphertext_opt) free(gcm_ciphertext_opt);
        if (gcm_decrypted_opt) free(gcm_decrypted_opt);
        return;
    }

    // 初始化测试数据
    for (size_t i = 0; i < gcm_data_size; i++) {
        gcm_data[i] = rand() & 0xFF;
    }

    sm4_gcm_generate_iv(iv);

    // 基础版GCM加密 - 简单计时
    start = clock();
    int result = sm4_gcm_encrypt(test_key, iv, 12, gcm_data, gcm_data_size,
                                 aad, aad_len, gcm_ciphertext, tag);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    if (result == 0) {
        printf("基础版GCM加密: %.8f秒 (%.2f MB/s)\n", 
               cpu_time_used, (gcm_data_size / 1024.0 / 1024.0) / cpu_time_used);

        // 基础版GCM解密 
        start = clock();
        result = sm4_gcm_decrypt(test_key, iv, 12, gcm_ciphertext, gcm_data_size,
                                 aad, aad_len, tag, gcm_decrypted);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

        if (result == 0) {
            printf("基础版GCM解密: %.8f秒 (%.2f MB/s)\n", 
                   cpu_time_used, (gcm_data_size / 1024.0 / 1024.0) / cpu_time_used);
            if (memcmp(gcm_data, gcm_decrypted, gcm_data_size) == 0) {
                printf("基础版GCM加解密验证通过\n");
            } else {
                printf("基础版GCM加解密验证失败\n");
            }
        } else {
            printf("基础版GCM解密失败\n");
        }
    } else {
        printf("基础版GCM加密失败\n");
    }

    // 优化版GCM加密 
    start = clock();
    result = sm4_gcm_encrypt_optimized(test_key, iv, 12, gcm_data, gcm_data_size,
                                       aad, aad_len, gcm_ciphertext_opt, tag_opt);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    if (result == 0) {
        printf("优化版GCM加密: %.8f秒 (%.2f MB/s)\n", 
               cpu_time_used, (gcm_data_size / 1024.0 / 1024.0) / cpu_time_used);

        // 优化版GCM解密 - 简单计时
        start = clock();
        result = sm4_gcm_decrypt_optimized(test_key, iv, 12, gcm_ciphertext_opt, gcm_data_size,
                                           aad, aad_len, tag_opt, gcm_decrypted_opt);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

        if (result == 0) {
            printf("优化版GCM解密: %.8f秒 (%.2f MB/s)\n", 
                   cpu_time_used, (gcm_data_size / 1024.0 / 1024.0) / cpu_time_used);
            if (memcmp(gcm_data, gcm_decrypted_opt, gcm_data_size) == 0) {
                printf("优化版GCM加解密验证通过\n");
            } else {
                printf("优化版GCM加解密验证失败\n");
            }
        } else {
            printf("优化版GCM解密失败\n");
        }
    } else {
        printf("优化版GCM加密失败\n");
    }

    // 释放内存
    free(gcm_data);
    free(gcm_ciphertext);
    free(gcm_decrypted);
    free(gcm_ciphertext_opt);
    free(gcm_decrypted_opt);
    printf("\n");
}

int main(void) {
    printf("SM4软件实现和优化测试程序\n");
    printf("========================\n\n");
    
    // 设置随机种子
    srand(time(NULL));
    
    // 运行基本SM4测试
    test_basic_sm4();
    test_ttable_sm4();
    test_aesni_sm4();
    test_gfni_sm4();
    test_vprold_sm4();
    test_avx512_gfni_sm4();
    test_avx512_vprold_sm4();
    performance_test();

    test_sm4_gcm();
    gcm_performance_test();
    
    printf("所有测试完成！\n");
    return 0;
} 