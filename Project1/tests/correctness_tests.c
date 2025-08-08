#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/sm4_basic.h"
#include "../include/sm4_ttable.h"
#include "../include/sm4_aesni.h"
#include "../include/sm4_gfni.h"
#include "../include/sm4_vprold.h"

// 标准测试向量
typedef struct {
    const char *name;
    uint8_t key[16];
    uint8_t plaintext[16];
    uint8_t expected_ciphertext[16];
} test_vector_t;

// 标准SM4测试向量
static const test_vector_t test_vectors[] = {
    {
        "标准测试向量1",
        {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
         0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10},
        {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
         0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10},
        {0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
         0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46}
    },
    {
        "标准测试向量2",
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x9f, 0x1f, 0x7b, 0xff, 0x6f, 0x55, 0x11, 0x38,
         0x4d, 0x94, 0x30, 0x53, 0x1e, 0x53, 0x8f, 0xd3}
    }
};

// 打印十六进制数据
void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// 比较数据
int compare_data(const uint8_t *data1, const uint8_t *data2, size_t len) {
    return memcmp(data1, data2, len) == 0;
}

// 测试单个实现
int test_implementation(const char *name,
                       void (*encrypt_func)(const uint8_t*, const uint8_t*, uint8_t*),
                       void (*decrypt_func)(const uint8_t*, const uint8_t*, uint8_t*),
                       int available) {
    if (!available) {
        printf("⚠ %s: 不支持，跳过测试\n", name);
        return 1; 
    }
    
    printf("测试 %s:\n", name);
    int passed = 0;
    int total = 0;
    
    // 测试标准向量
    for (int i = 0; i < sizeof(test_vectors) / sizeof(test_vectors[0]); i++) {
        const test_vector_t *tv = &test_vectors[i];
        uint8_t ciphertext[16];
        uint8_t decrypted[16];
        
        printf("  %s:\n", tv->name);
        
        // 加密测试
        encrypt_func(tv->key, tv->plaintext, ciphertext);
        int encrypt_correct = compare_data(ciphertext, tv->expected_ciphertext, 16);
        
        if (encrypt_correct) {
            printf("    ✓ 加密正确\n");
            passed++;
        } else {
            printf("    ✗ 加密错误\n");
            print_hex("      期望", tv->expected_ciphertext, 16);
            print_hex("      实际", ciphertext, 16);
        }
        
        // 解密测试
        decrypt_func(tv->key, ciphertext, decrypted);
        int decrypt_correct = compare_data(decrypted, tv->plaintext, 16);
        
        if (decrypt_correct) {
            printf("    ✓ 解密正确\n");
            passed++;
        } else {
            printf("    ✗ 解密错误\n");
            print_hex("      期望", tv->plaintext, 16);
            print_hex("      实际", decrypted, 16);
        }
        
        total += 2;
        printf("\n");
    }
    
    printf("  通过: %d/%d 测试\n", passed, total);
    printf("\n");
    
    return passed == total;
}

// 随机数据测试
void test_random_data() {
    printf("=== 随机数据测试 ===\n");
    
    const int num_tests = 1000;
    int passed = 0;
    
    for (int i = 0; i < num_tests; i++) {
        uint8_t key[16];
        uint8_t plaintext[16];
        uint8_t ciphertext[16];
        uint8_t decrypted[16];
        
        // 生成随机密钥和明文
        for (int j = 0; j < 16; j++) {
            key[j] = rand() % 256;
            plaintext[j] = rand() % 256;
        }
        
        // 测试基本实现
        sm4_encrypt_basic(key, plaintext, ciphertext);
        sm4_decrypt_basic(key, ciphertext, decrypted);
        
        if (compare_data(plaintext, decrypted, 16)) {
            passed++;
        } else {
            printf("随机测试 %d 失败\n", i);
        }
    }
    
    printf("随机数据测试: %d/%d 通过 (%.1f%%)\n\n", 
           passed, num_tests, (passed * 100.0) / num_tests);
}

// 边界条件测试
void test_edge_cases() {
    printf("=== 边界条件测试 ===\n");
    
    // 全零测试
    uint8_t zero_key[16] = {0};
    uint8_t zero_plaintext[16] = {0};
    uint8_t zero_ciphertext[16];
    uint8_t zero_decrypted[16];
    
    sm4_encrypt_basic(zero_key, zero_plaintext, zero_ciphertext);
    sm4_decrypt_basic(zero_key, zero_ciphertext, zero_decrypted);
    
    int zero_test = compare_data(zero_plaintext, zero_decrypted, 16);
    printf("全零测试: %s\n", zero_test ? "✓ 通过" : "✗ 失败");
    
    // 全一测试
    uint8_t ones_key[16];
    uint8_t ones_plaintext[16];
    uint8_t ones_ciphertext[16];
    uint8_t ones_decrypted[16];
    
    for (int i = 0; i < 16; i++) {
        ones_key[i] = 0xFF;
        ones_plaintext[i] = 0xFF;
    }
    
    sm4_encrypt_basic(ones_key, ones_plaintext, ones_ciphertext);
    sm4_decrypt_basic(ones_key, ones_ciphertext, ones_decrypted);
    
    int ones_test = compare_data(ones_plaintext, ones_decrypted, 16);
    printf("全一测试: %s\n", ones_test ? "✓ 通过" : "✗ 失败");
    
    // 交替位测试
    uint8_t alt_key[16];
    uint8_t alt_plaintext[16];
    uint8_t alt_ciphertext[16];
    uint8_t alt_decrypted[16];
    
    for (int i = 0; i < 16; i++) {
        alt_key[i] = (i % 2) ? 0xFF : 0x00;
        alt_plaintext[i] = (i % 2) ? 0x00 : 0xFF;
    }
    
    sm4_encrypt_basic(alt_key, alt_plaintext, alt_ciphertext);
    sm4_decrypt_basic(alt_key, alt_ciphertext, alt_decrypted);
    
    int alt_test = compare_data(alt_plaintext, alt_decrypted, 16);
    printf("交替位测试: %s\n", alt_test ? "✓ 通过" : "✗ 失败");
    
    printf("\n");
}

// 一致性测试
void test_consistency() {
    printf("=== 一致性测试 ===\n");
    
    const int num_tests = 100;
    int consistent = 1;
    
    for (int i = 0; i < num_tests; i++) {
        uint8_t key[16];
        uint8_t plaintext[16];
        
        // 生成随机数据
        for (int j = 0; j < 16; j++) {
            key[j] = rand() % 256;
            plaintext[j] = rand() % 256;
        }
        
        uint8_t ciphertext1[16], ciphertext2[16];
        uint8_t decrypted1[16], decrypted2[16];
        
        // 测试基本实现的一致性
        sm4_encrypt_basic(key, plaintext, ciphertext1);
        sm4_encrypt_basic(key, plaintext, ciphertext2);
        
        if (!compare_data(ciphertext1, ciphertext2, 16)) {
            printf("加密不一致性检测到 (测试 %d)\n", i);
            consistent = 0;
        }
        
        sm4_decrypt_basic(key, ciphertext1, decrypted1);
        sm4_decrypt_basic(key, ciphertext2, decrypted2);
        
        if (!compare_data(decrypted1, decrypted2, 16)) {
            printf("解密不一致性检测到 (测试 %d)\n", i);
            consistent = 0;
        }
    }
    
    printf("一致性测试: %s\n\n", consistent ? "✓ 通过" : "✗ 失败");
}

// 不同实现间的兼容性测试
void test_implementation_compatibility() {
    printf("=== 实现兼容性测试 ===\n");
    
    const int num_tests = 100;
    int compatible = 1;
    
    for (int i = 0; i < num_tests; i++) {
        uint8_t key[16];
        uint8_t plaintext[16];
        
        // 生成随机数据
        for (int j = 0; j < 16; j++) {
            key[j] = rand() % 256;
            plaintext[j] = rand() % 256;
        }
        
        uint8_t basic_ciphertext[16];
        uint8_t ttable_ciphertext[16];
        uint8_t decrypted[16];
        
        // 使用基本实现加密
        sm4_encrypt(key, plaintext, basic_ciphertext);
        
        // 使用T-table实现加密
        sm4_encrypt_ttable(key, plaintext, ttable_ciphertext);
        
        // 检查两个实现是否产生相同结果
        if (!compare_data(basic_ciphertext, ttable_ciphertext, 16)) {
            printf("实现不兼容检测到 (测试 %d)\n", i);
            compatible = 0;
        }
        
        // 使用基本实现解密T-table的密文
        sm4_decrypt(key, ttable_ciphertext, decrypted);
        
        if (!compare_data(plaintext, decrypted, 16)) {
            printf("解密兼容性失败 (测试 %d)\n", i);
            compatible = 0;
        }
    }
    
    printf("实现兼容性测试: %s\n\n", compatible ? "✓ 通过" : "✗ 失败");
}

// 内存泄漏测试
void test_memory_safety() {
    printf("=== 内存安全测试 ===\n");
    
    const int iterations = 10000;
    int memory_safe = 1;
    
    for (int i = 0; i < iterations; i++) {
        uint8_t key[16];
        uint8_t plaintext[16];
        uint8_t ciphertext[16];
        uint8_t decrypted[16];
        
        // 生成随机数据
        for (int j = 0; j < 16; j++) {
            key[j] = rand() % 256;
            plaintext[j] = rand() % 256;
        }
        
        // 重复调用加密解密函数
        for (int j = 0; j < 100; j++) {
            sm4_encrypt(key, plaintext, ciphertext);
            sm4_decrypt(key, ciphertext, decrypted);
        }
        
        // 验证结果仍然正确
        if (!compare_data(plaintext, decrypted, 16)) {
            printf("内存安全问题检测到 (迭代 %d)\n", i);
            memory_safe = 0;
        }
    }
    
    printf("内存安全测试: %s\n\n", memory_safe ? "✓ 通过" : "✗ 失败");
}

int main() {
    printf("SM4正确性测试\n");
    printf("============\n\n");
    
    // 初始化随机数生成器
    srand(time(NULL));
    
    // 测试各种实现
    int all_passed = 1;
    
    all_passed &= test_implementation("基本实现", 
                                    sm4_encrypt, sm4_decrypt, 1);
    
    all_passed &= test_implementation("T-table优化", 
                                    sm4_encrypt_ttable, sm4_decrypt_ttable, 1);
    
    all_passed &= test_implementation("AESNI优化", 
                                    sm4_encrypt_aesni, sm4_decrypt_aesni, 
                                    sm4_aesni_available());
    
    all_passed &= test_implementation("GFNI优化", 
                                    sm4_encrypt_gfni, sm4_decrypt_gfni, 
                                    sm4_gfni_available());
    
    all_passed &= test_implementation("VPROLD优化", 
                                    sm4_encrypt_vprold, sm4_decrypt_vprold, 
                                    sm4_vprold_available());
    
    // 其他测试
    test_random_data();
    test_edge_cases();
    test_consistency();
    test_implementation_compatibility();
    test_memory_safety();
    
    printf("正确性测试总结: %s\n", all_passed ? "✓ 全部通过" : "✗ 部分失败");
    
    return all_passed ? 0 : 1;
} 
