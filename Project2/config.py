"""
数字水印系统配置文件
"""

# import os  
from pathlib import Path

# 项目根目录
PROJECT_ROOT = Path(__file__).parent

# 数据目录
DATA_DIR = PROJECT_ROOT / "data"

# 统一输出目录（图片、图表等对外可见产物）
OUTPUTS_DIR = PROJECT_ROOT / "outputs"

# 确保目录存在
OUTPUTS_DIR.mkdir(exist_ok=True)

# 水印系统配置
WATERMARK_CONFIG = {
    # 默认水印强度
    "default_alpha": 0.15,
    # 检测阈值
    "default_threshold": 0.6,
    # 随机种子
    "default_seed": 42,
    # DCT块大小
    "dct_block_size": 8,
    # 中频系数位置
    "dct_coefficient": (4, 4),
}

# 鲁棒性测试配置
ROBUSTNESS_CONFIG = {
    # 旋转角度列表
    "rotation_angles": [5, 10, 15, 30, 45],
    # 翻转类型
    "flip_codes": [0, 1, -1],
    # 裁剪比例
    "crop_ratios": [0.9, 0.8, 0.7, 0.6],
    # 噪声强度
    "noise_levels": [0.05, 0.1, 0.15, 0.2],
    # 压缩质量
    "compression_qualities": [90, 80, 70, 60, 50],
    # 亮度对比度调整
    "brightness_contrast_pairs": [(0.8, 1.2), (1.2, 0.8), (0.7, 1.3), (1.3, 0.7)],
    # 模糊半径
    "blur_radii": [0.5, 1.0, 1.5, 2.0],
    # 缩放因子
    "scale_factors": [0.8, 0.9, 1.1, 1.2],
    # 平移参数
    "translation_offsets": [(10, 10), (20, 20), (30, 30), (50, 50), (-10, -10)],
}

# 文件路径配置
PATHS = {
    "sample_image": OUTPUTS_DIR / "sample_image.jpg",
    "watermarked_image": OUTPUTS_DIR / "watermarked_image.jpg",
    "comparison_image": OUTPUTS_DIR / "watermark_comparison.png",
    "robustness_report": OUTPUTS_DIR / "robustness_report.txt",
    "robustness_plot": OUTPUTS_DIR / "robustness_plot.png",
}

# 日志配置
LOGGING_CONFIG = {
    "version": 1,
    "disable_existing_loggers": False,
    "formatters": {
        "standard": {"format": "%(asctime)s [%(levelname)s] %(name)s: %(message)s"}
    },
    "handlers": {
        "default": {
            "level": "INFO",
            "formatter": "standard",
            "class": "logging.StreamHandler",
        },
        "file": {
            "level": "INFO",
            "formatter": "standard",
            "class": "logging.FileHandler",
            "filename": OUTPUTS_DIR / "watermark.log",
            "mode": "a",
        },
    },
    "loggers": {
        "": {"handlers": ["default", "file"], "level": "INFO", "propagate": False}
    },
}
