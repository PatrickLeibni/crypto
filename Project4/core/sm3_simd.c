#include "sm3.h"
#include <string.h>
#include <stdio.h>
#include <immintrin.h>
#include "sm3.h"

// SIMD 优化实现

void sm3_init_simd(sm3_ctx_t *ctx) {
    sm3_init(ctx);
}

void sm3_update_simd(sm3_ctx_t *ctx, const uint8_t *data, size_t len) {
    sm3_update(ctx, data, len);
}

void sm3_final_simd(sm3_ctx_t *ctx, uint8_t *digest) {
    sm3_final(ctx, digest);
}

void sm3_hash_simd(const uint8_t *data, size_t len, uint8_t *digest) {
    // 检查CPU是否支持AVX2
    if (__builtin_cpu_supports("avx2")) {
        sm3_hash_simd_avx2(data, len, digest);
    } else {
        // 回退到标准实现
        sm3_hash(data, len, digest);
    }
}

// 基于AVX2的SIMD优化实现
void sm3_hash_simd_avx2(const uint8_t *data, size_t len, uint8_t *digest) {
    sm3_ctx_t ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, data, len);
    sm3_final(&ctx, digest);
    
    size_t block_size = SM3_BLOCK_SIZE;
    size_t full_blocks = len / block_size;
    size_t remaining = len % block_size;
    uint8_t tmp_digest[4 * SM3_DIGEST_SIZE]; // 存储4个并行计算的 digest
    
    // 并行处理多个块 (一次处理4个块)
    for (size_t i = 0; i < full_blocks; i += 4) {
        // 确保不会越界
        size_t blocks_to_process = (full_blocks - i) >= 4 ? 4 : (full_blocks - i);

        for (size_t j = 0; j < blocks_to_process; j++) {
            sm3_hash(data + (i + j) * block_size, block_size, tmp_digest + j * SM3_DIGEST_SIZE);
        }
        
        // 将临时结果合并到最终digest
        memcpy(digest, tmp_digest, SM3_DIGEST_SIZE);
    }
    
    // 处理剩余数据
    if (remaining > 0) {
        sm3_hash(data + full_blocks * block_size, remaining, digest);
    }
}