"""
数字水印系统 - 基于DCT变换的图片水印嵌入和提取

主要功能:
- 水印嵌入和提取
- 鲁棒性测试
- 图片泄露检测
"""

__version__ = "1.0.0"
__author__ = "Digital Watermark Team"
__email__ = "watermark@example.com"

# 使用绝对导入，避免相对导入问题
try:
    from src.watermark import DigitalWatermark
    from src.robustness_test import RobustnessTester
except ImportError:
    # 如果src路径不可用，尝试直接导入
    try:
        from watermark import DigitalWatermark
        from robustness_test import RobustnessTester
    except ImportError:
        # 如果都失败，提供错误信息
        print("警告: 无法导入水印模块，请确保在正确的目录下运行")
        DigitalWatermark = None
        RobustnessTester = None

__all__ = ["DigitalWatermark", "RobustnessTester"] 