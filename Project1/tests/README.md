# SM4测试套件

本目录包含SM4加密算法的完整测试套件。

## 测试文件

- `unit_tests.c` - 单元测试
- `performance_tests.c` - 性能测试
- `correctness_tests.c` - 正确性测试
- `compatibility_tests.c` - 兼容性测试

## 运行测试

### 编译测试
```bash
make tests
```

### 运行所有测试
```bash
make test-all
```

### 运行特定测试
```bash
./unit_tests
./performance_tests
./correctness_tests
./compatibility_tests
```

## 测试覆盖

### 单元测试
- 基本加密解密功能
- 各种优化版本的正确性
- 边界条件测试
- 错误处理测试

### 性能测试
- 吞吐量测试
- 延迟测试
- 不同数据大小测试
- 优化版本对比测试

### 正确性测试
- 标准测试向量验证
- 随机数据测试
- 循环测试
- 一致性测试

### 兼容性测试
- 不同编译器测试
- 不同CPU架构测试
- 不同操作系统测试 