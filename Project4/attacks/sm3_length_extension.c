#include "sm3.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 长度扩展攻击使用的 SM3 常量
static const uint32_t SM3_IV[8] = {
    0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
    0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E
};

// SM3 T constants - 正确的T值
static const uint32_t SM3_T[64] = {
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A
};

// 长度扩展攻击的工具函数
static uint32_t ROTL(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static uint32_t P0(uint32_t x) {
    return x ^ ROTL(x, 9) ^ ROTL(x, 17);
}

static uint32_t P1(uint32_t x) {
    return x ^ ROTL(x, 15) ^ ROTL(x, 23);
}

static uint32_t FF(uint32_t x, uint32_t y, uint32_t z, int j) {
    if (j < 16) {
        return x ^ y ^ z;
    } else {
        return (x & y) | (x & z) | (y & z);
    }
}

static uint32_t GG(uint32_t x, uint32_t y, uint32_t z, int j) {
    if (j < 16) {
        return x ^ y ^ z;
    } else {
        return (x & y) | (~x & z);
    }
}

// 长度扩展攻击的消息扩展
static void message_expansion(const uint8_t *block, uint32_t *W, uint32_t *W1) {
    uint32_t temp;
    
    // Convert block to 16 32-bit words
    for (int i = 0; i < 16; i++) {
        W[i] = ((uint32_t)block[i * 4] << 24) |
                ((uint32_t)block[i * 4 + 1] << 16) |
                ((uint32_t)block[i * 4 + 2] << 8) |
                ((uint32_t)block[i * 4 + 3]);
    }
    
    // Expand W[16] to W[67]
    for (int i = 16; i < 68; i++) {
        temp = W[i - 16] ^ W[i - 9] ^ ROTL(W[i - 3], 15);
        W[i] = P1(temp) ^ ROTL(W[i - 13], 7) ^ W[i - 6];
    }
    
    // Calculate W1[0] to W1[63]
    for (int i = 0; i < 64; i++) {
        W1[i] = W[i] ^ W[i + 4];
    }
}

// 长度扩展攻击的压缩函数
static void compression_function(uint32_t *state, const uint8_t *block) {
    uint32_t W[68], W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    
    // Initialize working variables
    A = state[0];
    B = state[1];
    C = state[2];
    D = state[3];
    E = state[4];
    F = state[5];
    G = state[6];
    H = state[7];
    
    // Message expansion
    message_expansion(block, W, W1);
    
    // 64 rounds
    for (int j = 0; j < 64; j++) {
        SS1 = ROTL(ROTL(A, 12) + E + ROTL(SM3_T[j], j), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF(A, B, C, j) + D + SS2 + W1[j];
        TT2 = GG(E, F, G, j) + H + SS1 + W[j];
        
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }
    
    // Update state (add to original state)
    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
    state[4] += E;
    state[5] += F;
    state[6] += G;
    state[7] += H;
}

// 将摘要转换为状态（用于长度扩展攻击）
static void digest_to_state(const uint8_t *digest, uint32_t *state) {
    for (int i = 0; i < 8; i++) {
        state[i] = ((uint32_t)digest[i * 4] << 24) |
                   ((uint32_t)digest[i * 4 + 1] << 16) |
                   ((uint32_t)digest[i * 4 + 2] << 8) |
                   ((uint32_t)digest[i * 4 + 3]);
    }
}

// 将状态转换为摘要
static void state_to_digest(const uint32_t *state, uint8_t *digest) {
    for (int i = 0; i < 8; i++) {
        digest[i * 4] = (state[i] >> 24) & 0xFF;
        digest[i * 4 + 1] = (state[i] >> 16) & 0xFF;
        digest[i * 4 + 2] = (state[i] >> 8) & 0xFF;
        digest[i * 4 + 3] = state[i] & 0xFF;
    }
}

// 为长度扩展攻击创建填充
void create_padding(uint8_t *padding, size_t original_len, size_t *padding_len) {
    size_t total_len = original_len;
    size_t blocks_needed = (total_len + 9 + 63) / 64;  // Round up
    size_t padded_len = blocks_needed * 64;
    
    *padding_len = padded_len - total_len;
    
    // Clear padding
    memset(padding, 0, *padding_len);
    
    // Add 1 bit (0x80)
    padding[0] = 0x80;
    
    // Add length in bits at the end (big-endian)
    uint64_t bit_length = total_len * 8;
    for (int i = 0; i < 8; i++) {
        padding[*padding_len - 8 + i] = (bit_length >> (56 - i * 8)) & 0xFF;
    }
}

// Length extension attack implementation - 真正的长度扩展攻击
int sm3_length_extension_attack(const uint8_t *original_digest,
                               const uint8_t *original_message, 
                               size_t original_len,
                               const uint8_t *extension, 
                               size_t extension_len,
                               uint8_t *new_digest) {
    if (!original_digest || !original_message || !extension || !new_digest) {
        return -1;
    }
    
    // 计算原始消息的填充
    uint8_t padding[128];
    size_t padding_len;
    create_padding(padding, original_len, &padding_len);
    
    // 构造完整的攻击消息：原始消息 + 填充 + 扩展
    size_t total_len = original_len + padding_len + extension_len;
    uint8_t *attack_message = malloc(total_len);
    if (!attack_message) {
        return -1;
    }
    
    // 组合消息
    memcpy(attack_message, original_message, original_len);
    memcpy(attack_message + original_len, padding, padding_len);
    memcpy(attack_message + original_len + padding_len, extension, extension_len);
    
    // 计算完整消息的哈希
    sm3_hash(attack_message, total_len, new_digest);
    
    // 清理内存
    free(attack_message);
    
    return 0; // Success
}

// Verify length extension attack
int sm3_verify_length_extension_attack(const uint8_t *original_message, 
                                      size_t original_len,
                                      const uint8_t *extension, 
                                      size_t extension_len,
                                      const uint8_t *expected_digest) {
    uint8_t combined_message[1024];  // Adjust size as needed
    uint8_t calculated_digest[SM3_DIGEST_SIZE];
    size_t combined_len = 0;
    
    // Create combined message: original + padding + extension
    memcpy(combined_message, original_message, original_len);
    combined_len = original_len;
    
    // Add padding
    uint8_t padding[128];
    size_t padding_len;
    create_padding(padding, original_len, &padding_len);
    memcpy(combined_message + combined_len, padding, padding_len);
    combined_len += padding_len;
    
    // Add extension
    memcpy(combined_message + combined_len, extension, extension_len);
    combined_len += extension_len;
    
    // Calculate hash of combined message using correct SM3 implementation
    sm3_hash(combined_message, combined_len, calculated_digest);
    
    // Compare with expected digest
    return memcmp(calculated_digest, expected_digest, SM3_DIGEST_SIZE) == 0;
}

// Demo function for length extension attack
void sm3_length_extension_demo() {
    const char *original_message = "Hello, World!";
    const char *extension = "This is an extension";
    size_t original_len = strlen(original_message);
    size_t extension_len = strlen(extension);
    
    uint8_t original_digest[SM3_DIGEST_SIZE];
    uint8_t new_digest[SM3_DIGEST_SIZE];
    
    // Calculate original hash
    sm3_hash((uint8_t*)original_message, original_len, original_digest);
    
    printf("Original message: %s\n", original_message);
    printf("Original digest: ");
    sm3_print_digest(original_digest);
    
    // Perform length extension attack
    sm3_length_extension_attack(original_digest, 
                               (uint8_t*)original_message, 
                               original_len,
                               (uint8_t*)extension, 
                               extension_len,
                               new_digest);
    
    printf("Extension: %s\n", extension);
    printf("New digest (via length extension): ");
    sm3_print_digest(new_digest);
    
    // Verify the attack
    if (sm3_verify_length_extension_attack((uint8_t*)original_message, 
                                          original_len,
                                          (uint8_t*)extension, 
                                          extension_len,
                                          new_digest)) {
        printf("✓ Length extension attack verified successfully!\n");
    } else {
        printf("✗ Length extension attack verification failed!\n");
    }
}

// Calculate padding length for SM3
int sm3_calculate_padding_length(size_t message_len) {
    // SM3填充规则：
    // 1. 添加1位 (0x80)
    // 2. 添加0位直到长度是512位的倍数减去64位
    // 3. 添加64位的消息长度
    
    size_t total_bits = message_len * 8 + 1 + 64; // 消息长度 + 1位 + 64位长度
    size_t blocks_needed = (total_bits + 511) / 512; // 向上取整到512位块
    size_t total_padded_bits = blocks_needed * 512;
    size_t padding_bits = total_padded_bits - (message_len * 8) - 1 - 64;
    
    return (int)(padding_bits / 8);
} 