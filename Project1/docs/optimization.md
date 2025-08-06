# SM4优化技术详解

## 概述

本文档说明SM4算法实现中的各种优化技术，从查表优化到SIMD指令集优化。

## 优化策略

### 1. 算法级优化

#### T-table优化
- **原理**: 预计算T函数的所有可能输入输出
- **优势**: 减少运行时计算，提升性能2-3倍

```c
// T-table预计算
static uint32_t sm4_t_table[256];

void init_sm4_t_table() {
    for (int i = 0; i < 256; i++) {
        uint32_t sbox_out = sm4_sbox[i];
        sm4_t_table[i] = sbox_out ^ 
                         ROL(sbox_out, 2) ^ 
                         ROL(sbox_out, 10) ^ 
                         ROL(sbox_out, 18) ^ 
                         ROL(sbox_out, 24);
    }
}
```

### 2. 指令集优化

#### AESNI优化
- **适用**: Intel处理器（支持AESNI）
- **性能提升**: 3-5倍
- **核心**: 利用AES指令进行S盒替换

#### GFNI优化
- **适用**: 支持GFNI指令的处理器
- **性能提升**: 5-8倍
- **核心**: 使用伽罗瓦域新指令进行S盒计算

#### VPROLD优化
- **适用**: 支持VPROLD指令的最新处理器
- **性能提升**: 8-12倍
- **核心**: 向量化循环左移操作

### 3. 向量化优化

#### AVX-512优化
- **向量宽度**: 512位
- **并行度**: 同时处理16个32位字
- **性能提升**: 10-20倍

```c
// AVX-512向量化处理
static void sm4_encrypt_avx512_blocks(uint8_t *key, uint8_t *data, 
                                      uint8_t *out, size_t blocks) {
    __m512i key_schedule[32];
    sm4_key_expansion_avx512(key, key_schedule);
    
    for (size_t i = 0; i < blocks; i += 16) {
        __m512i block = _mm512_loadu_si256((__m512i*)(data + i * 16));
        block = sm4_encrypt_block_avx512(block, key_schedule);
        _mm512_storeu_si256((__m512i*)(out + i * 16), block);
    }
}
```

### 4. 内存优化

#### 数据对齐
```c
// 对齐内存分配
void *aligned_alloc(size_t size) {
    return _mm_malloc(size, 32);  // 32字节对齐
}
```

#### 缓存优化
- 使用连续内存布局
- 优化数据访问模式

### 5. 并行化优化

#### 多线程处理
```c
// OpenMP并行化
#pragma omp parallel for
for (size_t i = 0; i < total_blocks; i++) {
    sm4_encrypt_block(key, data + i * 16, out + i * 16);
}
```

## 性能测试

### 吞吐量测试
```c
void benchmark_throughput() {
    const size_t data_size = 1024 * 1024;  // 1MB
    uint8_t *data = malloc(data_size);
    uint8_t *out = malloc(data_size);
    
    clock_t start = clock();
    sm4_encrypt_blocks(key, data, out, data_size / 16);
    clock_t end = clock();
    
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    double throughput = data_size / time / 1024 / 1024;  // MB/s
    
    printf("吞吐量: %.2f MB/s\n", throughput);
}
```

### 延迟测试
```c
void benchmark_latency() {
    uint8_t block[16], out[16];
    
    clock_t start = clock();
    for (int i = 0; i < 1000000; i++) {
        sm4_encrypt_block(key, block, out);
    }
    clock_t end = clock();
    
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    double latency = time / 1000000 * 1000000;  // 微秒
    
    printf("平均延迟: %.2f 微秒\n", latency);
}
```

## 优化效果对比

| 优化方法 | 性能提升 | 内存开销 | 兼容性 |
|---------|---------|---------|--------|
| 基本实现 | 基准 | 低 | 高 |
| T-table | 2-3x | 中 | 高 |
| AESNI | 3-5x | 低 | 中 |
| GFNI | 5-8x | 低 | 中 |
| VPROLD | 8-12x | 低 | 低 |
| AVX-512 | 10-20x | 低 | 低 |
| 多线程 | 2-8x | 中 | 高 |

## 最佳实践

### 自动选择最优实现
```c
sm4_impl_t get_best_implementation() {
    if (cpu_supports_vprold()) {
        return SM4_IMPL_VPROLD;
    } else if (cpu_supports_gfni()) {
        return SM4_IMPL_GFNI;
    } else if (cpu_supports_aesni()) {
        return SM4_IMPL_AESNI;
    } else {
        return SM4_IMPL_TTABLE;
    }
}
```

### 渐进式优化
1. 首先确保正确性
2. 实现基本优化（T-table）
3. 添加指令集优化
4. 最后进行向量化优化

### 性能监控
- 定期进行性能测试
- 监控不同数据大小的性能
- 在不同硬件上验证优化效果 