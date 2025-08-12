# SM4 软件实现和优化

本项目实现了SM4加密算法的多种优化版本，从基本实现到SIMD优化，提供完整的性能测试和验证。

## 项目概述

SM4是中国国家密码管理局发布的分组密码算法，采用128位密钥和128位分组长度。本项目提供了从基本实现、各种优化、GCM模式实现的完整解决方案。

## 实现特性

### 核心实现
- **基本实现** (sm4_basic.c): 标准SM4算法，兼容性最佳
- **T-table优化** (sm4_ttable.c): 预计算优化，性能提升2-3倍
- **AESNI优化** (sm4_aesni.c): 利用Intel AES指令集，性能提升3-5倍
- **GFNI优化** (sm4_gfni.c): 使用伽罗瓦域新指令，适用于支持GFNI的处理器
- **VPROLD优化** (sm4_vprold.c): 使用最新的VPROLD指令集
- **AVX-512优化** (sm4_avx512_*.c): 512位向量批量处理，最高性能实现
- **GCM模式** (sm4_gcm.c): 支持SM4-GCM认证加密

## 项目结构

```
Project1/
├── include/                 # 头文件
├── src/                     # 源代码
│   ├── sm4_basic.c         # 基本实现
│   ├── sm4_ttable.c        # T-table优化
│   ├── sm4_aesni.c         # AESNI优化
│   ├── sm4_gfni.c          # GFNI优化
│   ├── sm4_vprold.c        # VPROLD优化
│   ├── sm4_avx512_gfni.c   # AVX-512+GFNI优化
│   ├── sm4_avx512_vprold.c # AVX-512+VPROLD优化
│   ├── sm4_gcm.c           # GCM模式实现
│   └── test_sm4.c          # 主测试程序
├── tests/                   # 测试文件
└── Makefile                # 编译配置
```

## 快速开始

### 环境检查
```bash
# 检查CPU特性支持
make check_cpu
```

### 编译和测试
```bash
# 编译主程序
make all

# 运行测试
make test

# 运行性能测试
make perf

# 运行完整测试
make test-all
```

## 使用示例

### 运行示例程序
```bash
# 编译所有示例
make examples

# 运行综合示例
./examples/run_example

详细使用说明请查看 [examples/README.md](examples/README.md)
完整编译测试可查看 [Makefile]

### 基本加密/解密
```c
#include "sm4_basic.h"

uint8_t key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                   0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
uint8_t plaintext[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                         0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
uint8_t ciphertext[16];

// 加密
sm4_encrypt(key, plaintext, ciphertext);

// 解密
uint8_t decrypted[16];
sm4_decrypt(key, ciphertext, decrypted);
```

### GCM模式使用
```c
#include "sm4_gcm.h"

uint8_t key[16], nonce[12], plaintext[16], ciphertext[16], tag[16];

// GCM加密
sm4_gcm_encrypt(key, nonce, plaintext, 16, ciphertext, tag);

// GCM解密
int result = sm4_gcm_decrypt(key, nonce, ciphertext, 16, tag, plaintext);
```

## 性能对比

| 实现方式 | 性能 (MB/s) | 性能提升 | 兼容性 |
|---------|-------------|---------|--------|
| 基本实现 | ~25 | 基准 | 高 |
| T-table | ~40 | 1.6x | 高 |
| AESNI | ~38 | 1.5x | 中 |
| AVX-512+GFNI | ~1653 | 66x | 低 |

## 常见问题

### Q: 编译时出现"不支持AVX-512"错误
A: 检查CPU是否支持AVX-512指令集，运行`make check_cpu`查看支持情况。

### Q: 性能测试显示"未找到性能数据"
A: 确保程序正确编译，检查是否有运行时错误。

### Q: 如何选择最佳实现？
A: 根据兼容性要求选择：
- 高兼容性：使用基本实现或T-table优化
- 中等兼容性：使用AESNI优化
- 高性能：使用AVX-512优化（需要支持相应指令集）

## 许可证

本项目采用MIT许可证。

## 贡献

欢迎提交Issue和Pull Request来改进项目。 
