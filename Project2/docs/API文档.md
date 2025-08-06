# API文档

## 1. DigitalWatermark类

### 1.1 类定义
```python
class DigitalWatermark:
    """数字水印系统 - 基于DCT变换的图片水印嵌入和提取"""
```

### 1.2 初始化参数
- `alpha` (float): 水印强度参数，范围0.01-0.5，默认0.15

### 1.3 主要方法

#### embed_watermark()
```python
def embed_watermark(self, image_path: str, message: str = None, 
                   output_path: str = None, seed: int = 42) -> Tuple[np.ndarray, np.ndarray]:
    """
    嵌入水印到图片中
    
    Args:
        image_path: 输入图片路径
        message: 水印消息
        output_path: 输出图片路径
        seed: 随机种子
        
    Returns:
        (原始图片, 含水印图片)
    """
```

#### extract_watermark()
```python
def extract_watermark(self, image_path: str, original_size: Tuple[int, int], 
                     watermark_size: int, seed: int = 42) -> np.ndarray:
    """
    从图片中提取水印
    
    Args:
        image_path: 含水印图片路径
        original_size: 原始图片尺寸 (height, width)
        watermark_size: 水印大小
        seed: 随机种子
        
    Returns:
        提取的水印序列
    """
```

#### detect_watermark()
```python
def detect_watermark(self, image_path: str, original_size: Tuple[int, int],
                    watermark_size: int, expected_message: str = None,
                    threshold: float = 0.6, seed: int = 42) -> Tuple[bool, float]:
    """
    检测图片中是否存在水印
    
    Args:
        image_path: 待检测图片路径
        original_size: 原始图片尺寸
        watermark_size: 水印大小
        expected_message: 期望的水印消息
        threshold: 检测阈值
        seed: 随机种子
        
    Returns:
        (是否检测到水印, 相似度)
    """
```

## 2. RobustnessTester类

### 2.1 类定义
```python
class RobustnessTester:
    """水印鲁棒性测试器 - 测试各种攻击对水印的影响"""
```

### 2.2 初始化参数
- `watermark_system` (DigitalWatermark): 水印系统实例

### 2.3 攻击方法

#### apply_rotation()
```python
def apply_rotation(self, image_path: str, angle: float, output_path: str = None) -> str:
    """
    应用旋转变换
    
    Args:
        image_path: 输入图片路径
        angle: 旋转角度 (度)
        output_path: 输出路径
        
    Returns:
        处理后的图片路径
    """
```

#### apply_flip()
```python
def apply_flip(self, image_path: str, flip_code: int, output_path: str = None) -> str:
    """
    应用翻转变换
    
    Args:
        image_path: 输入图片路径
        flip_code: 翻转类型 (0=垂直, 1=水平, -1=垂直+水平)
        output_path: 输出路径
        
    Returns:
        处理后的图片路径
    """
```

#### apply_noise()
```python
def apply_noise(self, image_path: str, noise_level: float, output_path: str = None) -> str:
    """
    添加噪声
    
    Args:
        image_path: 输入图片路径
        noise_level: 噪声强度 (0.01-0.5)
        output_path: 输出路径
        
    Returns:
        处理后的图片路径
    """
```

#### apply_compression()
```python
def apply_compression(self, image_path: str, quality: int, output_path: str = None) -> str:
    """
    应用JPEG压缩
    
    Args:
        image_path: 输入图片路径
        quality: 压缩质量 (1-100)
        output_path: 输出路径
        
    Returns:
        处理后的图片路径
    """
```

### 2.4 测试方法

#### run_comprehensive_test()
```python
def run_comprehensive_test(self, original_image: str, watermarked_image: str,
                         original_size: Tuple[int, int], watermark_size: int,
                         expected_message: str = None) -> Dict[str, Dict]:
    """
    运行全面的鲁棒性测试
    
    Args:
        original_image: 原始图片路径
        watermarked_image: 含水印图片路径
        original_size: 原始图片尺寸
        watermark_size: 水印大小
        expected_message: 期望的水印消息
        
    Returns:
        测试结果字典
    """
```

## 3. 工具函数

### 3.1 calculate_psnr()
```python
def calculate_psnr(original: np.ndarray, processed: np.ndarray) -> float:
    """
    计算峰值信噪比 (PSNR)
    
    Args:
        original: 原始图片
        processed: 处理后的图片
        
    Returns:
        PSNR值 (dB)
    """
```

### 3.2 calculate_ssim()
```python
def calculate_ssim(original: np.ndarray, processed: np.ndarray) -> float:
    """
    计算结构相似性指数 (SSIM)
    
    Args:
        original: 原始图片
        processed: 处理后的图片
        
    Returns:
        SSIM值 (0-1)
    """
```

### 3.3 create_test_image()
```python
def create_test_image(output_path: Path, size: Tuple[int, int] = (512, 512)) -> Path:
    """
    创建测试图片
    
    Args:
        output_path: 输出路径
        size: 图片尺寸 (width, height)
        
    Returns:
        图片路径
    """
```

## 4. 配置参数

### 4.1 WATERMARK_CONFIG
```python
WATERMARK_CONFIG = {
    "default_alpha": 0.15,        # 默认水印强度
    "default_threshold": 0.6,      # 默认检测阈值
    "default_seed": 42,           # 默认随机种子
    "dct_block_size": 8,          # DCT块大小
    "dct_coefficient": (4, 4),    # DCT系数位置
}
```

### 4.2 ROBUSTNESS_CONFIG
```python
ROBUSTNESS_CONFIG = {
    "rotation_angles": [5, 10, 15, 30, 45],           # 旋转角度
    "flip_codes": [0, 1, -1],                         # 翻转类型
    "crop_ratios": [0.9, 0.8, 0.7, 0.6],             # 裁剪比例
    "noise_levels": [0.05, 0.1, 0.15, 0.2],          # 噪声强度
    "compression_qualities": [90, 80, 70, 60, 50],    # 压缩质量
    "brightness_contrast_pairs": [                     # 亮度对比度
        (0.8, 1.2), (1.2, 0.8), (0.7, 1.3), (1.3, 0.7)
    ],
    "blur_radii": [0.5, 1.0, 1.5, 2.0],              # 模糊半径
    "scale_factors": [0.8, 0.9, 1.1, 1.2],           # 缩放因子
}
```

## 5. 使用示例

### 5.1 基本使用
```python
from src.watermark import DigitalWatermark
from src.robustness_test import RobustnessTester

# 初始化水印系统
watermark_system = DigitalWatermark(alpha=0.15)

# 嵌入水印
original_img, watermarked_img = watermark_system.embed_watermark(
    "input.jpg",
    message="My watermark",
    output_path="output.jpg"
)

# 检测水印
detected, similarity = watermark_system.detect_watermark(
    "output.jpg",
    original_size=(512, 512),
    watermark_size=4096,
    expected_message="My watermark"
)
```

### 5.2 鲁棒性测试
```python
# 初始化测试器
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

### 5.3 命令行使用
```bash
# 嵌入水印
python cli.py embed -i input.jpg -o output.jpg -m "My watermark"

# 检测水印
python cli.py detect -i output.jpg -m "My watermark"

# 鲁棒性测试
python cli.py test -i output.jpg -m "My watermark" --report report.txt
``` 