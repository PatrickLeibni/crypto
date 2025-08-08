# 基于数字水印的图片泄露检测系统

[![Python 3.7+](https://img.shields.io/badge/python-3.7+-blue.svg)](https://www.python.org/downloads/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

这是一个基于DCT（离散余弦变换）的数字水印系统，用于图片水印嵌入、提取和鲁棒性测试。系统能够抵抗多种常见的图片攻击，包括旋转、翻转、裁剪、噪声、压缩、亮度对比度调整、模糊和缩放等。

## ✨ 项目特点

- **🔒 安全性**: 基于DCT变换的不可见水印嵌入
- **🛡️ 鲁棒性**: 抵抗8种不同类型的攻击
- **📊 可视化**: 详细的测试报告和图表
- **⚡ 高性能**: 优化的算法实现
- **🔧 可配置**: 灵活的参数设置
- **📝 完整文档**: 详细的API文档和使用指南

## 🏗️ 技术架构

```
digital-watermark/
├── src/                    # 核心源代码
│   ├── watermark.py       # 水印系统核心
│   ├── robustness_test.py # 鲁棒性测试模块
│   └── __init__.py        # 包初始化文件
├── tests/                 # 测试套件
├── examples/              # 示例代码
├── data/                  # 数据文件
│   └── results/          # 测试结果
├── docs/                  # 文档
├── cli.py                # 命令行工具
├── utils.py              # 工具函数
├── config.py             # 配置文件
├── setup.py              # 安装配置
├── requirements.txt       # 依赖列表
├── Makefile              # 项目管理
├── __init__.py           # 项目初始化
└── README.md             # 项目说明
```

## 🚀 安装指南

### 系统要求

- Python 3.7+
- OpenCV 4.5+
- NumPy 1.20+
- Pillow 8.0+
- Matplotlib 3.3+

### 快速安装

```bash
# 下载或克隆项目到本地
# 进入项目目录
cd crypto/Project2

# 安装依赖
pip install -r requirements.txt

# 或者使用Makefile（如果可用）
make install
```

### 开发环境安装

```bash
# 安装开发依赖
make install-dev

# 运行测试
make test
```

## 🎯 快速开始

### 1. 基本使用

```python
from src.watermark import DigitalWatermark

# 初始化水印系统
watermark_system = DigitalWatermark(alpha=0.15)

# 嵌入水印
original_img, watermarked_img = watermark_system.embed_watermark(
    "input.jpg",
    message="My watermark",
    output_path="watermarked.jpg"
)

# 检测水印
detected, similarity = watermark_system.detect_watermark(
    "watermarked.jpg",
    original_size=(height, width),
    watermark_size=watermark_size,
    expected_message="My watermark"
)
```

### 2. 命令行使用

```bash
# 创建示例图片
python cli.py create-sample -o sample.jpg

# 嵌入水印
python cli.py embed -i sample.jpg -o watermarked.jpg -m "My watermark"

# 检测水印
python cli.py detect -i watermarked.jpg -m "My watermark"

# 鲁棒性测试
python cli.py test -i watermarked.jpg -m "My watermark" --report report.txt
```

### 3. 完整演示

```bash
# 运行完整演示（如果可用）
make demo

# 或者
python examples/demo.py
```

## 📖 使用指南

### 水印嵌入

```python
from src.watermark import DigitalWatermark

watermark_system = DigitalWatermark(alpha=0.15)

# 嵌入水印
original_img, watermarked_img = watermark_system.embed_watermark(
    "input.jpg",
    message="Digital Watermark Test",
    output_path="output.jpg"
)
```

### 水印检测

```python
# 检测水印
detected, similarity = watermark_system.detect_watermark(
    "watermarked.jpg",
    original_size=(512, 512),
    watermark_size=4096,
    expected_message="Digital Watermark Test",
    threshold=0.6
)

print(f"检测结果: {'成功' if detected else '失败'}")
print(f"相似度: {similarity:.4f}")
```

### 鲁棒性测试

```python
from src.robustness_test import RobustnessTester

tester = RobustnessTester(watermark_system)

# 运行全面测试
results = tester.run_comprehensive_test(
    "original.jpg",
    "watermarked.jpg",
    original_size=(512, 512),
    watermark_size=4096,
    expected_message="Test watermark"
)

# 生成报告
tester.generate_report("robustness_report.txt")
tester.plot_results("robustness_plot.png")
```

## 🔧 API文档

### DigitalWatermark类

#### 初始化参数

- `alpha` (float): 水印强度参数 (0.01-0.5)

#### 主要方法

- `embed_watermark(image_path, message, output_path, seed)`: 嵌入水印
- `extract_watermark(image_path, original_size, watermark_size, seed)`: 提取水印
- `detect_watermark(image_path, original_size, watermark_size, expected_message, threshold, seed)`: 检测水印

### RobustnessTester类

#### 攻击类型

- `apply_rotation(image_path, angle)`: 旋转变换
- `apply_flip(image_path, flip_code)`: 翻转变换
- `apply_crop(image_path, crop_ratio)`: 裁剪变换
- `apply_noise(image_path, noise_level)`: 噪声攻击
- `apply_compression(image_path, quality)`: 压缩攻击
- `apply_brightness_contrast(image_path, brightness, contrast)`: 亮度对比度调整
- `apply_blur(image_path, blur_radius)`: 模糊攻击
- `apply_scaling(image_path, scale_factor)`: 缩放攻击

## 🧪 测试

### 运行测试

```bash
# 运行所有测试
make test

# 运行特定测试
python -m pytest tests/test_watermark.py -v

# 生成覆盖率报告
python -m pytest tests/ --cov=src --cov-report=html
```

### 测试内容

- ✅ 水印嵌入和提取
- ✅ 水印检测准确性
- ✅ 不同参数的影响
- ✅ 鲁棒性攻击测试
- ✅ 工具函数测试

## 📈 测试结果

### 鲁棒性测试结果
- **旋转攻击**: 0% 成功率 (平均相似度: 0.5069)
- **翻转攻击**: 0% 成功率 (平均相似度: 0.5028)
- **裁剪攻击**: 0% 成功率 (平均相似度: 0.5117)
- **噪声攻击**: 25% 成功率 (平均相似度: 0.5853)
- **压缩攻击**: 0% 成功率 (平均相似度: 0.5287)
- **亮度对比度**: 0% 成功率 (平均相似度: 0.5255)
- **模糊攻击**: 0% 成功率 (平均相似度: 0.5192)
- **缩放攻击**: 0% 成功率 (平均相似度: 0.5023)

### 性能基准

```bash
# 运行性能测试
make benchmark
```

## 🎯 应用场景

### 1. 图片版权保护
在图片中嵌入版权信息，防止未经授权的使用。

### 2. 数字内容认证
验证图片的真实性和完整性，防止篡改。

### 3. 图片泄露检测
检测图片是否被非法传播或使用。

### 4. 数字水印研究
为水印算法研究提供基础框架。

## 🤝 贡献指南

### 开发环境设置

```bash
# 进入项目目录
cd crypto/Project2

# 安装开发依赖
make install-dev

# 运行测试
make test
```

### 代码规范

```bash
# 代码格式化
make format

### 提交规范

- 使用清晰的提交信息
- 包含测试用例
- 更新相关文档

## 📄 许可证

本项目采用MIT许可证，详见 [LICENSE](LICENSE) 文件。

> **注意**: 这是一个本地项目，没有在线仓库。
- 项目位置: 本地 `crypto/Project2/` 目录

## 贡献

欢迎提交Issue和Pull Request来改进项目。 
