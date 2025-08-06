#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <immintrin.h>

// SM4常量定义
#define SM4_BLOCK_SIZE 16
#define SM4_ROUNDS 32

// SM4 FK常量
static const uint32_t SM4_FK[4] = {
    0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
};

// SM4 CK常量
static const uint32_t SM4_CK[32] = {
    0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
    0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
    0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
    0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
    0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
    0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
    0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
    0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};

// SM4 S-box
static const uint8_t SM4_SBOX[256] = {
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
    0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
    0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
    0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
    0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
    0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
    0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
    0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
    0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
    0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
};

// 使用GFNI指令的S-box查找表
static __m512i sm4_sbox_gfni_table;

// 初始化GFNI S-box表
static void init_sm4_sbox_gfni(void) {
    static int initialized = 0;
    if (initialized) return;
    
    uint8_t sbox_table[64];
    for (int i = 0; i < 64; i++) {
        sbox_table[i] = SM4_SBOX[i];
    }
    
    sm4_sbox_gfni_table = _mm512_loadu_si512(sbox_table);
    initialized = 1;
}

// 使用GFNI指令的S-box查找
static __m512i sm4_sbox_gfni(__m512i x) {
    // 使用GFNI指令进行S-box查找
    // GFNI指令可以高效地进行字节级别的查找和替换
    return _mm512_gf2p8affine_epi64_epi8(x, sm4_sbox_gfni_table, 0);
}

// 使用AVX-512指令的循环左移
static __m512i sm4_rotl_avx512(__m512i x, int n) {
    return _mm512_rol_epi32(x, n);
}

// 使用AVX-512的SM4线性变换L
static __m512i sm4_l_avx512(__m512i x) {
    return _mm512_xor_si512(x,
           _mm512_xor_si512(sm4_rotl_avx512(x, 2),
           _mm512_xor_si512(sm4_rotl_avx512(x, 10),
           _mm512_xor_si512(sm4_rotl_avx512(x, 18),
                           sm4_rotl_avx512(x, 24)))));
}

// 使用AVX-512和GFNI的SM4 T函数
static __m512i sm4_t_avx512_gfni(__m512i x) {
    __m512i sbox_result = sm4_sbox_gfni(x);
    return sm4_l_avx512(sbox_result);
}

// 循环左移函数（标量版本）
static uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// SM4线性变换L（标量版本）
static uint32_t sm4_l(uint32_t x) {
    return x ^ rotl(x, 2) ^ rotl(x, 10) ^ rotl(x, 18) ^ rotl(x, 24);
}

// SM4线性变换L'（标量版本）
static uint32_t sm4_l_prime(uint32_t x) {
    return x ^ rotl(x, 13) ^ rotl(x, 23);
}

// SM4 T'函数（标量版本）
static uint32_t sm4_t_prime(uint32_t x) {
    uint32_t result = 0;
    result |= ((uint32_t)SM4_SBOX[(x >> 24) & 0xFF]) << 24;
    result |= ((uint32_t)SM4_SBOX[(x >> 16) & 0xFF]) << 16;
    result |= ((uint32_t)SM4_SBOX[(x >> 8) & 0xFF]) << 8;
    result |= ((uint32_t)SM4_SBOX[x & 0xFF]);
    return sm4_l_prime(result);
}

// SM4密钥扩展
void sm4_key_expansion_advanced(const uint8_t *key, uint32_t *rk) {
    uint32_t mk[4];
    uint32_t k[36];
    
    // 将密钥转换为32位字
    for (int i = 0; i < 4; i++) {
        mk[i] = ((uint32_t)key[4*i] << 24) | ((uint32_t)key[4*i+1] << 16) |
                 ((uint32_t)key[4*i+2] << 8) | ((uint32_t)key[4*i+3]);
    }
    
    // 密钥扩展
    for (int i = 0; i < 4; i++) {
        k[i] = mk[i] ^ SM4_FK[i];
    }
    
    for (int i = 0; i < 32; i++) {
        k[i + 4] = k[i] ^ sm4_t_prime(k[i + 1] ^ k[i + 2] ^ k[i + 3] ^ SM4_CK[i]);
        rk[i] = k[i + 4];
    }
}

// 使用AVX-512和GFNI的SM4加密/解密（并行处理多个块）
void sm4_crypt_avx512_gfni(const uint8_t *input, uint8_t *output, const uint32_t *rk, int encrypt, int num_blocks) {
    init_sm4_sbox_gfni();
    
    for (int block = 0; block < num_blocks; block += 16) {
        int blocks_this_round = (num_blocks - block >= 16) ? 16 : (num_blocks - block);
        if (blocks_this_round < 16) {
            // 剩余不足16块，降级为普通SM4
            for (int b = 0; b < blocks_this_round; b++) {
                // 这里调用普通SM4加解密
                uint8_t key_bytes[16];
                // 这里假设rk是32轮子密钥，直接用rk即可
                // 需要区分加密/解密方向
                if (encrypt) {
                    sm4_crypt(&input[(block + b) * SM4_BLOCK_SIZE], &output[(block + b) * SM4_BLOCK_SIZE], rk, 1);
                } else {
                    sm4_crypt(&input[(block + b) * SM4_BLOCK_SIZE], &output[(block + b) * SM4_BLOCK_SIZE], rk, 0);
                }
            }
            break;
        }
        __m512i x[4];
        // 加载16个块的数据
        for (int i = 0; i < 4; i++) {
            x[i] = _mm512_loadu_si512(&input[(block * SM4_BLOCK_SIZE) + i * 16]);
        }
        // 32轮变换
        for (int round = 0; round < 32; round++) {
            int rk_idx = encrypt ? round : (31 - round);
            __m512i temp = _mm512_xor_si512(x[0], 
                           sm4_t_avx512_gfni(_mm512_xor_si512(_mm512_xor_si512(x[1], x[2]), 
                                                              _mm512_xor_si512(x[3], _mm512_set1_epi32(rk[rk_idx])))));
            x[0] = x[1];
            x[1] = x[2];
            x[2] = x[3];
            x[3] = temp;
        }
        // 反序输出
        for (int i = 0; i < 4; i++) {
            _mm512_storeu_si512(&output[(block * SM4_BLOCK_SIZE) + i * 16], x[3-i]);
        }
    }
}

// 使用AVX-512和GFNI的SM4加密
void sm4_encrypt_avx512_gfni(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext, int num_blocks) {
    uint32_t rk[32];
    sm4_key_expansion_advanced(key, rk);
    sm4_crypt_avx512_gfni(plaintext, ciphertext, rk, 1, num_blocks);
}

// 使用AVX-512和GFNI的SM4解密
void sm4_decrypt_avx512_gfni(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext, int num_blocks) {
    uint32_t rk[32];
    sm4_key_expansion_advanced(key, rk);
    sm4_crypt_avx512_gfni(ciphertext, plaintext, rk, 0, num_blocks);
}

// 检查AVX-512和GFNI支持
int sm4_avx512_gfni_available(void) {
    return __builtin_cpu_supports("avx512f") && __builtin_cpu_supports("gfni");
} 