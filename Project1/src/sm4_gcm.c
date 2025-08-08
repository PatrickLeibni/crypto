#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

// 1. 128位无符号整数类型
typedef struct {
    uint64_t high;
    uint64_t low;
} uint128_t;

// 伽罗瓦域规约多项式 R
static const uint128_t R = {0, 0xE100000000000000};

// 128位整数操作
static inline uint128_t uint128_xor(uint128_t a, uint128_t b) {
    uint128_t res;
    res.high = a.high ^ b.high;
    res.low = a.low ^ b.low;
    return res;
}

static inline int uint128_eq(uint128_t a, uint128_t b) {
    return a.high == b.high && a.low == b.low;
}

static inline uint128_t uint128_rshift1(uint128_t x) {
    uint128_t res;
    res.low = (x.low >> 1) | (x.high << 63);
    res.high = x.high >> 1;
    return res;
}

static inline int uint128_lsb(uint128_t x) {
    return x.low & 1;
}

// 2. 辅助函数 (字节转换)
static uint128_t bytes_to_uint128(const uint8_t* bytes, size_t offset) {
    uint128_t res = {0, 0};
    for (int i = 0; i < 8; ++i) {
        res.high = (res.high << 8) | bytes[offset + i];
    }
    for (int i = 0; i < 8; ++i) {
        res.low = (res.low << 8) | bytes[offset + 8 + i];
    }
    return res;
}

static void uint128_to_bytes(uint128_t n, uint8_t* bytes) {
    for (int i = 7; i >= 0; --i) {
        bytes[i] = (n.high >> ((7 - i) * 8)) & 0xFF;
    }
    for (int i = 7; i >= 0; --i) {
        bytes[8 + i] = (n.low >> ((7 - i) * 8)) & 0xFF;
    }
}

// 3. SM4加密实现
static const uint8_t SM4_SBOX[256] = {
    0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
    0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
    0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
    0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
    0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
    0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
    0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
    0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
    0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
    0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
    0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
    0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
    0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
    0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
    0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
    0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48
};

static uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static uint32_t sm4_t(uint32_t x) {
    uint8_t b[4];
    for(int i = 0; i < 4; ++i) {
        b[i] = (x >> ((3 - i) * 8)) & 0xFF;
    }
    uint32_t sbox_out = ((uint32_t)SM4_SBOX[b[0]] << 24) | 
                        ((uint32_t)SM4_SBOX[b[1]] << 16) | 
                        ((uint32_t)SM4_SBOX[b[2]] << 8) | 
                        SM4_SBOX[b[3]];
    return sbox_out ^ rotl(sbox_out, 2) ^ rotl(sbox_out, 10) ^ 
           rotl(sbox_out, 18) ^ rotl(sbox_out, 24);
}

static void sm4_set_key(const uint8_t* key, uint32_t* rk) {
    uint32_t K[4];
    
    for(int i = 0; i < 4; ++i) {
        K[i] = ((uint32_t)key[4*i] << 24) | ((uint32_t)key[4*i+1] << 16) | 
                ((uint32_t)key[4*i+2] << 8) | key[4*i+3];
    }
    
    for(int i = 0; i < 32; ++i) {
        uint32_t T_arg = K[1] ^ K[2] ^ K[3] ^ (0x00010203 + i*0x01010101);
        uint8_t b[4];
        for(int j = 0; j < 4; ++j) {
            b[j] = (T_arg >> ((3-j)*8)) & 0xff;
        }
        uint32_t sbox_out = ((uint32_t)SM4_SBOX[b[0]] << 24) | 
                            ((uint32_t)SM4_SBOX[b[1]] << 16) | 
                            ((uint32_t)SM4_SBOX[b[2]] << 8) | 
                            SM4_SBOX[b[3]];
        uint32_t L_prime = sbox_out ^ rotl(sbox_out, 13) ^ rotl(sbox_out, 23);
        
        K[0] ^= L_prime;
        rk[i] = K[0];
        
        // 循环左移
        uint32_t temp = K[0];
        K[0] = K[1]; K[1] = K[2]; K[2] = K[3]; K[3] = temp;
    }
}

static void sm4_crypt_ecb(const uint8_t* in, uint8_t* out, const uint32_t* rk) {
    uint32_t X[4];
    for(int i = 0; i < 4; ++i) {
        X[i] = ((uint32_t)in[4*i] << 24) | ((uint32_t)in[4*i+1] << 16) | 
                ((uint32_t)in[4*i+2] << 8) | in[4*i+3];
    }
    
    for(int r = 0; r < 32; ++r) {
        uint32_t T_out = sm4_t(X[1] ^ X[2] ^ X[3] ^ rk[r]);
        X[0] ^= T_out;
        
        // 循环左移
        uint32_t temp = X[0];
        X[0] = X[1]; X[1] = X[2]; X[2] = X[3]; X[3] = temp;
    }
    
    for(int i = 0; i < 4; ++i) {
        out[4*i]   = (X[3-i] >> 24) & 0xFF;
        out[4*i+1] = (X[3-i] >> 16) & 0xFF;
        out[4*i+2] = (X[3-i] >> 8) & 0xFF;
        out[4*i+3] = X[3-i] & 0xFF;
    }
}

// 4. GCM 实现
static uint128_t gf_mult_slow(uint128_t a, uint128_t b) {
    uint128_t res = {0, 0};
    uint128_t v = a;
    
    for (int i = 0; i < 128; ++i) {
        if ((b.high >> (63 - (i / 64) * 64)) & (1ULL << (63 - i % 64))) {
            res = uint128_xor(res, v);
        }
        if (uint128_lsb(v)) {
            v = uint128_xor(uint128_rshift1(v), R);
        } else {
            v = uint128_rshift1(v);
        }
    }
    return res;
}

// 生成快速乘法表
static void generate_gmult_tables(const uint128_t H, uint128_t* tables) {
    // 初始化表 - 计算 H * i 对于 i = 0 到 15
    for (int i = 0; i < 16; ++i) {
        uint128_t val = {0, (uint64_t)i};
        tables[i] = gf_mult_slow(val, H);
    }
    
    // 生成其他表项 - 每个表项是前一个表项右移4位的结果
    for (int k = 1; k < 32; ++k) {
        for (int i = 0; i < 16; ++i) {
            uint128_t val = tables[(k-1) * 16 + i];
            // 右移4位（相当于乘以x^4）
            for (int s = 0; s < 4; ++s) {
                if (uint128_lsb(val)) {
                    val = uint128_xor(uint128_rshift1(val), R);
                } else {
                    val = uint128_rshift1(val);
                }
            }
            tables[k * 16 + i] = val;
        }
    }
}

// 使用查表法的快速乘法
static uint128_t gf_mult_fast(uint128_t x, const uint128_t* tables) {
    uint128_t res = {0, 0};
    
    // 对每个4位块使用查表
    for (int k = 0; k < 16; ++k) {
        uint8_t byte_l = (x.low >> (k * 4)) & 0xF;
        uint8_t byte_h = (x.high >> (k * 4)) & 0xF;
        res = uint128_xor(res, tables[k * 16 + byte_l]);
        res = uint128_xor(res, tables[(k + 16) * 16 + byte_h]);
    }
    return res;
}

// SM4-GCM 上下文结构
typedef struct {
    uint128_t H;
    uint128_t J0;
    uint32_t round_keys[32];
    uint8_t keystream_block[16];
    uint128_t ghash_tables[512]; // 预计算表
    int use_optimization;
} sm4_gcm_ctx_t;

// 初始化SM4-GCM
static int sm4_gcm_init(sm4_gcm_ctx_t* ctx, const uint8_t* key, const uint8_t* iv, int use_optimization) {
    if (!ctx || !key || !iv) return -1;
    
    // 初始化结构体
    memset(ctx, 0, sizeof(sm4_gcm_ctx_t));
    ctx->use_optimization = use_optimization;
    
    // 设置SM4密钥
    sm4_set_key(key, ctx->round_keys);
    
    // 计算H值
    uint8_t zero_block[16] = {0};
    uint8_t h_bytes[16];
    sm4_crypt_ecb(zero_block, h_bytes, ctx->round_keys);
    ctx->H = bytes_to_uint128(h_bytes, 0);
    
    // 计算J0
    uint8_t j0_bytes[16];
    memcpy(j0_bytes, iv, 12);
    j0_bytes[12] = 0; j0_bytes[13] = 0; j0_bytes[14] = 0; j0_bytes[15] = 1;
    ctx->J0 = bytes_to_uint128(j0_bytes, 0);
    
    // 如果使用优化，生成查表
    if (use_optimization) {
        generate_gmult_tables(ctx->H, ctx->ghash_tables);
    }
    
    return 0;
}

// 清理SM4-GCM上下文
static void sm4_gcm_cleanup(sm4_gcm_ctx_t* ctx) {
    (void)ctx;
}

// GHASH函数
static uint128_t ghash(sm4_gcm_ctx_t* ctx, const uint8_t* aad, size_t aad_len, 
                       const uint8_t* ciphertext, size_t ct_len) {
    // 计算需要的总长度
    size_t aad_padded = aad_len + (16 - (aad_len % 16)) % 16;
    size_t ct_padded = ct_len + (16 - (ct_len % 16)) % 16;
    size_t total_len = aad_padded + ct_padded + 16; // 包括长度字段
    
    uint8_t* ghash_input = malloc(total_len);
    if (!ghash_input) return (uint128_t){0, 0};
    
    size_t offset = 0;
    
    // 添加AAD
    if (aad && aad_len > 0) {
        memcpy(ghash_input + offset, aad, aad_len);
        offset += aad_len;
    }
    
    // 填充到16字节边界
    while (offset % 16 != 0) {
        ghash_input[offset++] = 0;
    }
    
    // 添加密文
    if (ciphertext && ct_len > 0) {
        memcpy(ghash_input + offset, ciphertext, ct_len);
        offset += ct_len;
    }
    
    // 填充到16字节边界
    while (offset % 16 != 0) {
        ghash_input[offset++] = 0;
    }
    
    // 添加长度字段
    uint64_t aad_len_bits = aad_len * 8;
    uint64_t ct_len_bits = ct_len * 8;
    for(int i = 7; i >= 0; --i) {
        ghash_input[offset++] = (aad_len_bits >> (i * 8)) & 0xff;
    }
    for(int i = 7; i >= 0; --i) {
        ghash_input[offset++] = (ct_len_bits >> (i * 8)) & 0xff;
    }
    
    // 计算GHASH
    uint128_t y = {0, 0};
    for (size_t i = 0; i < offset; i += 16) {
        uint128_t block = bytes_to_uint128(ghash_input, i);
        y = uint128_xor(y, block);
        y = gf_mult_slow(y, ctx->H);
    }
    
    free(ghash_input);
    return y;
}

// 优化的GHASH计算 - 使用查表法
static uint128_t ghash_optimized(const uint8_t* aad, size_t aad_len, 
                                 const uint8_t* ciphertext, size_t ct_len,
                                 const uint128_t* tables) {
    // 计算需要的总长度
    size_t aad_padded = aad_len + (16 - (aad_len % 16)) % 16;
    size_t ct_padded = ct_len + (16 - (ct_len % 16)) % 16;
    size_t total_len = aad_padded + ct_padded + 16; // 包括长度字段
    
    uint8_t* ghash_input = malloc(total_len);
    if (!ghash_input) return (uint128_t){0, 0};
    
    size_t offset = 0;
    
    // 添加AAD
    if (aad && aad_len > 0) {
        memcpy(ghash_input + offset, aad, aad_len);
        offset += aad_len;
    }
    
    // 填充到16字节边界
    while (offset % 16 != 0) {
        ghash_input[offset++] = 0;
    }
    
    // 添加密文
    if (ciphertext && ct_len > 0) {
        memcpy(ghash_input + offset, ciphertext, ct_len);
        offset += ct_len;
    }
    
    // 填充到16字节边界
    while (offset % 16 != 0) {
        ghash_input[offset++] = 0;
    }
    
    // 添加长度字段
    uint64_t aad_len_bits = aad_len * 8;
    uint64_t ct_len_bits = ct_len * 8;
    for(int i = 7; i >= 0; --i) {
        ghash_input[offset++] = (aad_len_bits >> (i * 8)) & 0xff;
    }
    for(int i = 7; i >= 0; --i) {
        ghash_input[offset++] = (ct_len_bits >> (i * 8)) & 0xff;
    }
    
    // 计算GHASH（使用查表优化）
    uint128_t y = {0, 0};
    for (size_t i = 0; i < offset; i += 16) {
        uint128_t block = bytes_to_uint128(ghash_input, i);
        y = uint128_xor(y, block);
        
        // 使用查表法进行快速乘法
        y = gf_mult_fast(y, tables);
    }
    
    free(ghash_input);
    return y;
}

// SM4-GCM加密
static int sm4_gcm_encrypt_internal(sm4_gcm_ctx_t* ctx, const uint8_t* aad, size_t aad_len,
                                   const uint8_t* plaintext, size_t pt_len,
                                   uint8_t* ciphertext, uint8_t* tag) {
    if (!ctx || !plaintext || !ciphertext || !tag) return -1;
    
    // 生成密钥流并加密
    uint128_t counter_block = ctx->J0;
    // 第一个计数器是J0+1
    uint32_t* counter_val = (uint32_t*)&counter_block.low;
    *counter_val = __builtin_bswap32(__builtin_bswap32(*counter_val) + 1);
    
    for (size_t i = 0; i < pt_len; ++i) {
        if (i % 16 == 0) {
            uint8_t counter_bytes[16];
            uint128_to_bytes(counter_block, counter_bytes);
            sm4_crypt_ecb(counter_bytes, ctx->keystream_block, ctx->round_keys);
            
            // 增加计数器（下一个块）
            if (i + 16 < pt_len) {
                *counter_val = __builtin_bswap32(__builtin_bswap32(*counter_val) + 1);
            }
        }
        ciphertext[i] = plaintext[i] ^ ctx->keystream_block[i % 16];
    }
    
    // 计算认证标签
    uint128_t ghash_val;
    if (ctx->use_optimization) {
        ghash_val = ghash_optimized(aad, aad_len, ciphertext, pt_len, ctx->ghash_tables);
    } else {
        ghash_val = ghash(ctx, aad, aad_len, ciphertext, pt_len);
    }
    
    // 生成标签掩码
    uint8_t tag_mask_bytes[16];
    uint128_to_bytes(ctx->J0, tag_mask_bytes);
    sm4_crypt_ecb(tag_mask_bytes, tag_mask_bytes, ctx->round_keys);
    uint128_t tag_mask = bytes_to_uint128(tag_mask_bytes, 0);
    
    // 计算最终标签
    uint128_t tag_val = uint128_xor(ghash_val, tag_mask);
    uint128_to_bytes(tag_val, tag);
    
    return 0;
}

// SM4-GCM解密
static int sm4_gcm_decrypt_internal(sm4_gcm_ctx_t* ctx, const uint8_t* aad, size_t aad_len,
                                   const uint8_t* ciphertext, size_t ct_len,
                                   const uint8_t* tag, uint8_t* plaintext) {
    if (!ctx || !ciphertext || !plaintext || !tag) return -1;
    
    // 先计算认证标签
    uint128_t ghash_val;
    if (ctx->use_optimization) {
        ghash_val = ghash_optimized(aad, aad_len, ciphertext, ct_len, ctx->ghash_tables);
    } else {
        ghash_val = ghash(ctx, aad, aad_len, ciphertext, ct_len);
    }
    
    // 生成标签掩码
    uint8_t tag_mask_bytes[16];
    uint128_to_bytes(ctx->J0, tag_mask_bytes);
    sm4_crypt_ecb(tag_mask_bytes, tag_mask_bytes, ctx->round_keys);
    uint128_t tag_mask = bytes_to_uint128(tag_mask_bytes, 0);
    
    // 计算期望标签
    uint128_t expected_tag = uint128_xor(ghash_val, tag_mask);
    uint8_t expected_tag_bytes[16];
    uint128_to_bytes(expected_tag, expected_tag_bytes);
    
    // 验证标签
    if (memcmp(tag, expected_tag_bytes, 16) != 0) {
        return -1; // 认证失败
    }
    
    // 标签验证通过，进行解密
    uint128_t counter_block = ctx->J0;
    // 第一个计数器是J0+1
    uint32_t* counter_val = (uint32_t*)&counter_block.low;
    *counter_val = __builtin_bswap32(__builtin_bswap32(*counter_val) + 1);
    
    for (size_t i = 0; i < ct_len; ++i) {
        if (i % 16 == 0) {
            uint8_t counter_bytes[16];
            uint128_to_bytes(counter_block, counter_bytes);
            sm4_crypt_ecb(counter_bytes, ctx->keystream_block, ctx->round_keys);
            
            // 增加计数器（下一个块）
            if (i + 16 < ct_len) {
                *counter_val = __builtin_bswap32(__builtin_bswap32(*counter_val) + 1);
            }
        }
        plaintext[i] = ciphertext[i] ^ ctx->keystream_block[i % 16];
    }
    
    return 0;
}

// 5. 公共API实现
// 生成IV
void sm4_gcm_generate_iv(uint8_t *iv) {
    if (!iv) return;
    
    // 使用时间戳和随机数生成IV
    srand(time(NULL));
    for (int i = 0; i < 12; ++i) {
        iv[i] = rand() & 0xFF;
    }
}

// 基础版本加密
int sm4_gcm_encrypt(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                     const uint8_t *plaintext, size_t plaintext_len,
                     const uint8_t *aad, size_t aad_len,
                     uint8_t *ciphertext, uint8_t *tag) {
    if (!key || !iv || !plaintext || !ciphertext || !tag) return -1;
    if (iv_len != 12) return -1;
    
    sm4_gcm_ctx_t ctx;
    int result = sm4_gcm_init(&ctx, key, iv, 0);
    if (result != 0) return result;
    
    result = sm4_gcm_encrypt_internal(&ctx, aad, aad_len, plaintext, plaintext_len, 
                                     ciphertext, tag);
    
    sm4_gcm_cleanup(&ctx);
    return result;
}

// 基础版本解密
int sm4_gcm_decrypt(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                     const uint8_t *ciphertext, size_t ciphertext_len,
                     const uint8_t *aad, size_t aad_len,
                     const uint8_t *tag, uint8_t *plaintext) {
    if (!key || !iv || !ciphertext || !plaintext || !tag) return -1;
    if (iv_len != 12) return -1;
    
    sm4_gcm_ctx_t ctx;
    int result = sm4_gcm_init(&ctx, key, iv, 0);
    if (result != 0) return result;
    
    result = sm4_gcm_decrypt_internal(&ctx, aad, aad_len, ciphertext, ciphertext_len, 
                                     tag, plaintext);
    
    sm4_gcm_cleanup(&ctx);
    return result;
}

// 优化版本加密
int sm4_gcm_encrypt_optimized(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                               const uint8_t *plaintext, size_t plaintext_len,
                               const uint8_t *aad, size_t aad_len,
                               uint8_t *ciphertext, uint8_t *tag) {
    if (!key || !iv || !plaintext || !ciphertext || !tag) return -1;
    if (iv_len != 12) return -1;
    
    sm4_gcm_ctx_t ctx;
    int result = sm4_gcm_init(&ctx, key, iv, 1);
    if (result != 0) return result;
    
    result = sm4_gcm_encrypt_internal(&ctx, aad, aad_len, plaintext, plaintext_len, 
                                     ciphertext, tag);
    
    sm4_gcm_cleanup(&ctx);
    return result;
}

// 优化版本解密
int sm4_gcm_decrypt_optimized(const uint8_t *key, const uint8_t *iv, size_t iv_len,
                               const uint8_t *ciphertext, size_t ciphertext_len,
                               const uint8_t *aad, size_t aad_len,
                               const uint8_t *tag, uint8_t *plaintext) {
    if (!key || !iv || !ciphertext || !plaintext || !tag) return -1;
    if (iv_len != 12) return -1;
    
    sm4_gcm_ctx_t ctx;
    int result = sm4_gcm_init(&ctx, key, iv, 1);
    if (result != 0) return result;
    
    result = sm4_gcm_decrypt_internal(&ctx, aad, aad_len, ciphertext, ciphertext_len, 
                                     tag, plaintext);
    
    sm4_gcm_cleanup(&ctx);
    return result;
} 
