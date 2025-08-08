"""
数字水印系统 - 基于DCT变换的图片水印嵌入和提取

主要功能:
- 水印嵌入和提取
- 鲁棒性测试
- 图片泄露检测
"""

try:
    from src.watermark import DigitalWatermark
    from src.robustness_test import RobustnessTester
except ImportError:
    try:
        from watermark import DigitalWatermark
        from robustness_test import RobustnessTester
    except ImportError:
        print("警告: 无法导入水印模块，请确保在正确的目录下运行")
        DigitalWatermark = None
        RobustnessTester = None

__all__ = ["DigitalWatermark", "RobustnessTester"] 
