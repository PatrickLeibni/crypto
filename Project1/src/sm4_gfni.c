#include "sm4_gfni.h"
#include <string.h>
#include <cpuid.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <immintrin.h>

// 1. SM4 S-box 
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

// 2. 基本SM4函数
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

// 3. GFNI优化实现
int sm4_gfni_available(void) {
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid(7, &eax, &ebx, &ecx, &edx)) {
        return (ebx & (1 << 8)) != 0; // GFNI位
    }
    return 0;
}

// GFNI优化的S盒替换
static __m256i sm4_sbox_gfni_256(__m256i x) {
    // 使用GFNI指令进行S盒替换
    __m256i sbox_table = _mm256_setr_epi8(
        0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
        0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99
    );
    
    // 使用GFNI进行字节级替换
    return _mm256_gf2p8affine_epi64_epi8(x, sbox_table, 0);
}

static __m512i sm4_sbox_gfni_512(__m512i x) {
    // 512位版本的GFNI S盒替换
    __m512i sbox_table = _mm512_set1_epi8(0xd6);
    
    return _mm512_gf2p8affine_epi64_epi8(x, sbox_table, 0);
}

// GFNI优化的T函数
static __m256i sm4_t_gfni_256(__m256i x) {
    // 使用GFNI进行S盒替换
    __m256i sbox_out = sm4_sbox_gfni_256(x);
    
    // 循环左移操作
    __m256i rot2 = _mm256_rol_epi32(sbox_out, 2);
    __m256i rot10 = _mm256_rol_epi32(sbox_out, 10);
    __m256i rot18 = _mm256_rol_epi32(sbox_out, 18);
    __m256i rot24 = _mm256_rol_epi32(sbox_out, 24);
    
    return _mm256_xor_si256(_mm256_xor_si256(_mm256_xor_si256(sbox_out, rot2), rot10),
                            _mm256_xor_si256(rot18, rot24));
}

static __m512i sm4_t_gfni_512(__m512i x) {
    // 512位版本的GFNI T函数
    __m512i sbox_out = sm4_sbox_gfni_512(x);
    
    __m512i rot2 = _mm512_rol_epi32(sbox_out, 2);
    __m512i rot10 = _mm512_rol_epi32(sbox_out, 10);
    __m512i rot18 = _mm512_rol_epi32(sbox_out, 18);
    __m512i rot24 = _mm512_rol_epi32(sbox_out, 24);
    
    return _mm512_xor_si512(_mm512_xor_si512(_mm512_xor_si512(sbox_out, rot2), rot10),
                            _mm512_xor_si512(rot18, rot24));
}

void sm4_encrypt_gfni(const uint8_t *key, const uint8_t *plaintext, uint8_t *ciphertext) {
    uint32_t rk[32];
    sm4_set_key(key, rk);
    
    // 加载数据到256位向量
    __m256i X[4];
    for(int i = 0; i < 4; ++i) {
        X[i] = _mm256_set1_epi32(
            ((uint32_t)plaintext[4*i] << 24) | 
            ((uint32_t)plaintext[4*i+1] << 16) | 
            ((uint32_t)plaintext[4*i+2] << 8) | 
            plaintext[4*i+3]
        );
    }
    
    for(int r = 0; r < 32; ++r) {
        __m256i T_arg = _mm256_xor_si256(_mm256_xor_si256(X[1], X[2]), 
                                         _mm256_xor_si256(X[3], _mm256_set1_epi32(rk[r])));
        
        __m256i T_out = sm4_t_gfni_256(T_arg);
        X[0] = _mm256_xor_si256(X[0], T_out);
        
        // 循环左移
        __m256i temp = X[0];
        X[0] = X[1]; X[1] = X[2]; X[2] = X[3]; X[3] = temp;
    }
    
    // 存储结果
    uint32_t result[4];
    for(int i = 0; i < 4; ++i) {
        result[i] = _mm256_extract_epi32(X[3-i], 0);
    }
    
    for(int i = 0; i < 4; ++i) {
        ciphertext[4*i]   = (result[i] >> 24) & 0xFF;
        ciphertext[4*i+1] = (result[i] >> 16) & 0xFF;
        ciphertext[4*i+2] = (result[i] >> 8) & 0xFF;
        ciphertext[4*i+3] = result[i] & 0xFF;
    }
}

void sm4_decrypt_gfni(const uint8_t *key, const uint8_t *ciphertext, uint8_t *plaintext) {
    uint32_t rk[32];
    sm4_set_key(key, rk);
    
    // 解密时轮密钥顺序相反
    uint32_t rk_reverse[32];
    for(int i = 0; i < 32; ++i) {
        rk_reverse[i] = rk[31 - i];
    }
    
    __m256i X[4];
    for(int i = 0; i < 4; ++i) {
        X[i] = _mm256_set1_epi32(
            ((uint32_t)ciphertext[4*i] << 24) | 
            ((uint32_t)ciphertext[4*i+1] << 16) | 
            ((uint32_t)ciphertext[4*i+2] << 8) | 
            ciphertext[4*i+3]
        );
    }
    
    for(int r = 0; r < 32; ++r) {
        __m256i T_arg = _mm256_xor_si256(_mm256_xor_si256(X[1], X[2]), 
                                         _mm256_xor_si256(X[3], _mm256_set1_epi32(rk_reverse[r])));
        
        __m256i T_out = sm4_t_gfni_256(T_arg);
        X[0] = _mm256_xor_si256(X[0], T_out);
        
        __m256i temp = X[0];
        X[0] = X[1]; X[1] = X[2]; X[2] = X[3]; X[3] = temp;
    }
    
    uint32_t result[4];
    for(int i = 0; i < 4; ++i) {
        result[i] = _mm256_extract_epi32(X[3-i], 0);
    }
    
    for(int i = 0; i < 4; ++i) {
        plaintext[4*i]   = (result[i] >> 24) & 0xFF;
        plaintext[4*i+1] = (result[i] >> 16) & 0xFF;
        plaintext[4*i+2] = (result[i] >> 8) & 0xFF;
        plaintext[4*i+3] = result[i] & 0xFF;
    }
}

// 批量GFNI优化
void sm4_encrypt_gfni_batch(const uint8_t *key, const uint8_t *plaintext, 
                             uint8_t *ciphertext, int num_blocks) {
    uint32_t rk[32];
    sm4_set_key(key, rk);
    
    for(int block = 0; block < num_blocks; ++block) {
        const uint8_t *in = plaintext + block * 16;
        uint8_t *out = ciphertext + block * 16;
        
        // 使用512位向量处理
        __m512i data = _mm512_loadu_si512(in);
        
        for(int r = 0; r < 32; ++r) {
            __m512i round_key = _mm512_set1_epi32(rk[r]);
            data = _mm512_xor_si512(data, round_key);
            data = sm4_t_gfni_512(data);
        }
        
        _mm512_storeu_si512(out, data);
    }
}

void sm4_decrypt_gfni_batch(const uint8_t *key, const uint8_t *ciphertext, 
                             uint8_t *plaintext, int num_blocks) {
    uint32_t rk[32];
    sm4_set_key(key, rk);
    
    uint32_t rk_reverse[32];
    for(int i = 0; i < 32; ++i) {
        rk_reverse[i] = rk[31 - i];
    }
    
    for(int block = 0; block < num_blocks; ++block) {
        const uint8_t *in = ciphertext + block * 16;
        uint8_t *out = plaintext + block * 16;
        
        __m512i data = _mm512_loadu_si512(in);
        
        for(int r = 0; r < 32; ++r) {
            __m512i round_key = _mm512_set1_epi32(rk_reverse[r]);
            data = _mm512_xor_si512(data, round_key);
            data = sm4_t_gfni_512(data);
        }
        
        _mm512_storeu_si512(out, data);
    }
} 
