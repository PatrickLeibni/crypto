"""
数字水印系统工具函数
"""

import cv2
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

# import logging  # 未使用，注释掉
from typing import Tuple, Dict, Any
import json
import time

from config import LOGGING_CONFIG


def setup_logging():
    """设置日志系统"""
    import logging.config

    logging.config.dictConfig(LOGGING_CONFIG)
    return logging.getLogger(__name__)


def calculate_psnr(original: np.ndarray, processed: np.ndarray) -> float:
    """
    计算峰值信噪比 (PSNR)

    Args:
        original: 原始图片
        processed: 处理后的图片

    Returns:
        PSNR值 (dB)
    """
    mse = np.mean((original.astype(np.float64) - processed.astype(np.float64)) ** 2)
    if mse == 0:
        return float("inf")
    return 20 * np.log10(255.0 / np.sqrt(mse))


def calculate_ssim(original: np.ndarray, processed: np.ndarray) -> float:
    """
    计算结构相似性指数 (SSIM)

    Args:
        original: 原始图片
        processed: 处理后的图片

    Returns:
        SSIM值 (0-1)
    """
    # 简化的SSIM计算
    mu_x = np.mean(original)
    mu_y = np.mean(processed)

    sigma_x = np.std(original)
    sigma_y = np.std(processed)
    sigma_xy = np.mean((original - mu_x) * (processed - mu_y))

    c1 = (0.01 * 255) ** 2
    c2 = (0.03 * 255) ** 2

    ssim = ((2 * mu_x * mu_y + c1) * (2 * sigma_xy + c2)) / (
        (mu_x**2 + mu_y**2 + c1) * (sigma_x**2 + sigma_y**2 + c2)
    )

    return ssim


def create_test_image(output_path: Path, size: Tuple[int, int] = (512, 512)) -> Path:
    """
    创建测试图片

    Args:
        output_path: 输出路径
        size: 图片尺寸 (width, height)

    Returns:
        图片路径
    """
    # 创建渐变图片
    img = np.zeros((size[1], size[0], 3), dtype=np.uint8)

    # 创建渐变效果
    for i in range(size[1]):
        for j in range(size[0]):
            img[i, j] = [
                int(255 * i / size[1]),  # 红色渐变
                int(255 * j / size[0]),  # 绿色渐变
                int(255 * (i + j) / (size[0] + size[1])),  # 蓝色渐变
            ]

    # 添加几何图形
    cv2.circle(img, (size[0] // 4, size[1] // 4), 50, (255, 255, 255), -1)
    cv2.rectangle(
        img,
        (size[0] // 2, size[1] // 2),
        (size[0] // 2 + 100, size[1] // 2 + 100),
        (0, 255, 0),
        3,
    )
    cv2.line(img, (0, 0), (size[0], size[1]), (255, 0, 255), 5)

    # 添加文字
    font = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img, "Digital Watermark Test", (50, 50), font, 1, (255, 255, 255), 2)

    cv2.imwrite(str(output_path), img)
    return output_path


def save_results(results: Dict[str, Any], output_path: Path):
    """
    保存测试结果

    Args:
        results: 测试结果字典
        output_path: 输出路径
    """
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(results, f, indent=2, ensure_ascii=False)


def load_results(input_path: Path) -> Dict[str, Any]:
    """
    加载测试结果

    Args:
        input_path: 输入路径

    Returns:
        测试结果字典
    """
    with open(input_path, "r", encoding="utf-8") as f:
        return json.load(f)


def plot_watermark_comparison(
    original: np.ndarray,
    watermarked: np.ndarray,
    output_path: Path,
    title: str = "水印对比",
):
    """
    绘制水印对比图

    Args:
        original: 原始图片
        watermarked: 含水印图片
        output_path: 输出路径
        title: 图表标题
    """
    # 转换为RGB
    if len(original.shape) == 3:
        original_rgb = cv2.cvtColor(original, cv2.COLOR_BGR2RGB)
        watermarked_rgb = cv2.cvtColor(watermarked, cv2.COLOR_BGR2RGB)
    else:
        original_rgb = original
        watermarked_rgb = watermarked

    # 计算差异
    diff = cv2.absdiff(original, watermarked)
    if len(diff.shape) == 3:
        diff_rgb = cv2.cvtColor(diff, cv2.COLOR_BGR2RGB)
    else:
        diff_rgb = diff

    # 创建图表
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    axes[0].imshow(original_rgb)
    axes[0].set_title("原始图片")
    axes[0].axis("off")

    axes[1].imshow(watermarked_rgb)
    axes[1].set_title("含水印图片")
    axes[1].axis("off")

    axes[2].imshow(diff_rgb)
    axes[2].set_title("差异图")
    axes[2].axis("off")

    plt.suptitle(title, fontsize=16)
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches="tight")
    plt.close()


def benchmark_performance(func, *args, **kwargs) -> Tuple[Any, float]:
    """
    性能基准测试

    Args:
        func: 要测试的函数
        *args: 函数参数
        **kwargs: 函数关键字参数

    Returns:
        (函数结果, 执行时间)
    """
    start_time = time.time()
    result = func(*args, **kwargs)
    end_time = time.time()

    return result, end_time - start_time


def validate_image_path(image_path: Path) -> bool:
    """
    验证图片路径

    Args:
        image_path: 图片路径

    Returns:
        是否有效
    """
    if not image_path.exists():
        return False

    # 检查是否为图片文件
    img = cv2.imread(str(image_path))
    return img is not None


def get_image_info(image_path: Path) -> Dict[str, Any]:
    """
    获取图片信息

    Args:
        image_path: 图片路径

    Returns:
        图片信息字典
    """
    img = cv2.imread(str(image_path))
    if img is None:
        return {}

    return {
        "path": str(image_path),
        "shape": img.shape,
        "dtype": str(img.dtype),
        "size": img.size,
        "channels": img.shape[2] if len(img.shape) == 3 else 1,
        "file_size": image_path.stat().st_size,
    }
