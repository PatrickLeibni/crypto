# Project4: SM3哈希算法实现与优化

## 📁 项目结构

```
Project4/
├── core/                    # 核心SM3实现
│   ├── sm3_basic.c         # 基本SM3实现
│   ├── sm3_optimized.c     # 优化SM3实现
│   └── sm3_simd.c          # SIMD优化实现
├── attacks/                 # 攻击相关实现
│   └── sm3_length_extension.c  # 长度扩展攻击
├── merkle/                  # Merkle树实现
│   └── sm3_merkle.c        # RFC6962 Merkle树
├── utils/                   # 工具函数
│   └── sm3_utils.c         # 通用工具函数
├── src/                     # 主程序
│   └── main.c              # 主程序入口
├── tests/                   # 测试文件
│   ├── test_sm3_basic.c    # 基本功能测试
│   ├── test_length_extension.c  # 长度扩展攻击测试
│   ├── test_merkle_tree.c  # Merkle树测试
│   └── test_performance.c  # 性能测试
├── examples/                # 示例程序
│   ├── basic_hash_example.c
│   ├── length_extension_example.c
│   └── merkle_tree_example.c
├── include/                 # 头文件
│   └── sm3.h              # SM3接口定义
└── docs/                   # 文档
```

## 🎯 项目要求完成情况

### ✅ a) SM3软件实现效率改进
- **基本实现** (`core/sm3_basic.c`) - 标准SM3实现
- **优化实现** (`core/sm3_optimized.c`) - 性能优化版本
- **SIMD实现** (`core/sm3_simd.c`) - SIMD指令优化
- **性能对比**: SIMD实现最快 (209.24 MB/s)

### ✅ b) Length-Extension Attack验证
- **攻击实现** (`attacks/sm3_length_extension.c`) - 成功实现SM3长度扩展攻击
- **攻击效率**: 比正常计算快8.9%
- **测试覆盖**: 多种消息和扩展组合测试

### ✅ c) 基于SM3的Merkle树 (RFC6962)
- **10万叶子节点**: 成功构建大规模Merkle树
- **存在性证明**: 验证成功，证明创建<1ms
- **不存在性证明**: 验证成功
- **性能优秀**: 构建时间0.07秒，证明验证<1ms

## 🚀 快速开始

### 编译项目
```bash
make clean
make all
```

### 运行完整测试
```bash
make test-all
```

### 运行单个测试
```bash
make test-basic              # 基本SM3功能测试
make test-length-extension   # 长度扩展攻击测试
make test-merkle            # Merkle树测试
make test-performance       # 性能测试
```

### 运行示例程序
```bash
make run-examples
```

## 📊 性能测试结果

### SM3哈希性能
- **SIMD实现**: 46.67 微秒/次 (209.24 MB/s) - 最快
- **基本实现**: 48.70 微秒/次 (200.55 MB/s)
- **优化版实现**: 50.57 微秒/次 (193.11 MB/s)

### 长度扩展攻击性能
- **攻击效率**: 比正常计算快8.9%
- **测试覆盖**: 100%成功率

### Merkle树性能 (10万叶子节点)
- **叶子哈希生成**: 0.04秒
- **Merkle树构建**: 0.07秒
- **存在性证明**: <1ms创建，<1ms验证
- **不存在性证明**: <1ms创建，<1ms验证

## 🏗️ 技术实现

### SM3核心算法
- 基于国密标准SM3
- 支持基本、优化、SIMD三种实现
- 完整的测试向量验证

### 长度扩展攻击
- 利用SM3的Merkle-Damgård结构
- 无需原始消息即可扩展哈希
- 验证了SM3的脆弱性

### Merkle树 (RFC6962)
- 支持10万叶子节点
- 存在性和不存在性证明
- 高效的内存管理和性能优化

## 📝 文件说明

### 核心实现
- `core/sm3_basic.c`: 标准SM3实现，易于理解
- `core/sm3_optimized.c`: 性能优化版本
- `core/sm3_simd.c`: SIMD指令优化版本

### 攻击实现
- `attacks/sm3_length_extension.c`: 长度扩展攻击实现

### Merkle树实现
- `merkle/sm3_merkle.c`: RFC6962标准Merkle树实现

### 工具函数
- `utils/sm3_utils.c`: 通用工具函数和辅助功能

## 🧪 测试覆盖

### 基本功能测试
- 空字符串、单字符、短消息、长消息
- 基本实现和优化实现对比
- 与OpenSSL SM3实现结果一致

### 长度扩展攻击测试
- 多种消息和扩展组合
- 攻击成功率验证
- 性能对比测试

### Merkle树测试
- 小规模测试 (8节点)
- 中规模测试 (1000节点)
- 大规模测试 (10000节点)
- 超大规模测试 (100000节点)
- 存在性和不存在性证明

## 📈 性能优化

### SM3实现优化
1. **基本实现**: 标准算法实现
2. **优化实现**: 循环展开、常量优化
3. **SIMD实现**: 向量化指令优化

### Merkle树优化
1. **内存管理**: 高效的内存分配和释放
2. **证明优化**: 快速证明生成和验证
3. **树结构**: 优化的树构建算法

## 贡献

欢迎提交Issue和Pull Request来改进项目。 

