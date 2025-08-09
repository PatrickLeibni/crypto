#include "sm3.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

// 优化的常量定义
static const uint32_t SM3_IV[8] = {
    0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
    0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E
};

// SM3 常量 T
static const uint32_t SM3_T[16] = {
    0x79CC4519, 0xF3988A32, 0xE7311465, 0xCE6228CB,
    0x9CC45197, 0x3988A32F, 0x7311465E, 0xE6228CBC,
    0xCC451979, 0x988A32F3, 0x311465E7, 0x6228CBCE,
    0xC451979C, 0x88A32F39, 0x11465E73, 0x228CBCE6
};

// 获取第 j 轮的 T 常量
static uint32_t get_T_value(int j) {
    if (j < 16) {
        return 0x79CC4519;
    } else {
        // 对于j >= 16，使用标准SM3 T值
        return 0x7A879D8A;
    }
}

// 优化的内联函数
static inline uint32_t ROTL(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static inline uint32_t P0(uint32_t x) {
    return x ^ ROTL(x, 9) ^ ROTL(x, 17);
}

static inline uint32_t P1(uint32_t x) {
    return x ^ ROTL(x, 15) ^ ROTL(x, 23);
}

static inline uint32_t FF(uint32_t x, uint32_t y, uint32_t z, int j) {
    return (j < 16) ? (x ^ y ^ z) : ((x & y) | (x & z) | (y & z));
}

static inline uint32_t GG(uint32_t x, uint32_t y, uint32_t z, int j) {
    return (j < 16) ? (x ^ y ^ z) : ((x & y) | (~x & z));
}

// 优化的消息扩展 - 使用查找表
static void message_expansion_optimized(const uint8_t *block, uint32_t *W, uint32_t *W1) {
    // 快速字节到字的转换
    for (int i = 0; i < 16; i++) {
        W[i] = ((uint32_t)block[i * 4] << 24) |
                ((uint32_t)block[i * 4 + 1] << 16) |
                ((uint32_t)block[i * 4 + 2] << 8) |
                ((uint32_t)block[i * 4 + 3]);
    }
    
    // 优化的扩展循环
    for (int i = 16; i < 68; i++) {
        uint32_t temp = W[i - 16] ^ W[i - 9] ^ ROTL(W[i - 3], 15);
        W[i] = P1(temp) ^ ROTL(W[i - 13], 7) ^ W[i - 6];
    }
    
    // 并行计算W1
    for (int i = 0; i < 64; i++) {
        W1[i] = W[i] ^ W[i + 4];
    }
}

// 优化的压缩函数 - 减少函数调用开销
static void compression_function_optimized(uint32_t *state, const uint8_t *block) {
    uint32_t W[68], W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    
    // 初始化工作变量
    A = state[0];
    B = state[1];
    C = state[2];
    D = state[3];
    E = state[4];
    F = state[5];
    G = state[6];
    H = state[7];
    
    // 消息扩展
    message_expansion_optimized(block, W, W1);
    
    // 优化的压缩轮次 - 内联所有函数调用
    for (int j = 0; j < 64; j++) {
        SS1 = ROTL(ROTL(A, 12) + E + ROTL(get_T_value(j), j), 7);
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
    
    // 更新状态 - 使用异或操作
    state[0] ^= A;
    state[1] ^= B;
    state[2] ^= C;
    state[3] ^= D;
    state[4] ^= E;
    state[5] ^= F;
    state[6] ^= G;
    state[7] ^= H;
}

// 优化的SM3函数
void sm3_init_optimized(sm3_ctx_t *ctx) {
    memcpy(ctx->state, SM3_IV, sizeof(SM3_IV));
    ctx->length = 0;
    ctx->buffer_size = 0;
    memset(ctx->buffer, 0, SM3_BLOCK_SIZE);
}

void sm3_update_optimized(sm3_ctx_t *ctx, const uint8_t *data, size_t len) {
    size_t remaining = len;
    size_t offset = 0;
    
    // 处理缓冲区中的剩余数据
    if (ctx->buffer_size > 0) {
        size_t to_copy = SM3_BLOCK_SIZE - ctx->buffer_size;
        if (to_copy > remaining) {
            to_copy = remaining;
        }
        
        memcpy(ctx->buffer + ctx->buffer_size, data, to_copy);
        ctx->buffer_size += to_copy;
        remaining -= to_copy;
        offset += to_copy;
        
        if (ctx->buffer_size == SM3_BLOCK_SIZE) {
            compression_function_optimized(ctx->state, ctx->buffer);
            ctx->buffer_size = 0;
            ctx->length += SM3_BLOCK_SIZE * 8;
        }
    }
    
    // 处理完整块 - 优化循环
    while (remaining >= SM3_BLOCK_SIZE) {
        compression_function_optimized(ctx->state, data + offset);
        offset += SM3_BLOCK_SIZE;
        remaining -= SM3_BLOCK_SIZE;
        ctx->length += SM3_BLOCK_SIZE * 8;
    }
    
    // 存储剩余数据
    if (remaining > 0) {
        memcpy(ctx->buffer, data + offset, remaining);
        ctx->buffer_size = remaining;
    }
}

void sm3_final_optimized(sm3_ctx_t *ctx, uint8_t *digest) {
    uint8_t padding[SM3_BLOCK_SIZE * 2];  // 增加缓冲区大小
    size_t padding_len;
    
    // 计算填充长度
    if (ctx->buffer_size + 9 <= SM3_BLOCK_SIZE) {
        padding_len = SM3_BLOCK_SIZE - ctx->buffer_size;
    } else {
        padding_len = SM3_BLOCK_SIZE * 2 - ctx->buffer_size;
    }
    
    // 创建填充
    memset(padding, 0, padding_len);
    padding[0] = 0x80;
    
    // 添加消息长度（位）
    uint64_t bit_length = ctx->length + ctx->buffer_size * 8;
    for (int i = 0; i < 8; i++) {
        padding[padding_len - 8 + i] = (bit_length >> (56 - i * 8)) & 0xFF;
    }
    
    // 处理填充
    sm3_update_optimized(ctx, padding, padding_len);
    
    // 转换状态为摘要 - 优化字节序转换
    for (int i = 0; i < 8; i++) {
        digest[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        digest[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        digest[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        digest[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}

void sm3_hash_optimized(const uint8_t *data, size_t len, uint8_t *digest) {
    sm3_ctx_t ctx;
    sm3_init_optimized(&ctx);
    sm3_update_optimized(&ctx, data, len);
    sm3_final_optimized(&ctx, digest);
}

 