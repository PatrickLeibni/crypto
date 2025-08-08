"""
数字水印系统工具函数
"""

import cv2
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

# import logging  
from typing import Tuple, Dict, Any
import json
import time

from config import LOGGING_CONFIG


def configure_chinese_fonts(preferred_fonts=None):
    """
    配置 Matplotlib 中文字体，避免中文渲染为方块。

    会在系统可用字体中按优先级选择第一个可用的中文字体。
    默认优先级列表包含常见的中文字体名称。
    """
    import os
    import matplotlib
    from matplotlib import font_manager as fm
    from config import PROJECT_ROOT

    if preferred_fonts is None:
        preferred_fonts = [
            "Noto Sans CJK SC",
            "Noto Sans CJK",
            "Noto Sans SC",
            "Source Han Sans SC",
            "Source Han Sans CN",
            "SimHei",
            "WenQuanYi Zen Hei",
            "WenQuanYi Micro Hei",
            "Microsoft YaHei",
            "Arial Unicode MS",
        ]

    # 收集系统已安装字体名
    available_font_names = set(f.name for f in fm.fontManager.ttflist)

    # 尝试优先使用外部字体文件（环境变量或本地 fonts 目录）
    chosen = None

    # 1) 环境变量指定字体文件路径
    env_font_path = os.environ.get("CHINESE_FONT_PATH")
    if env_font_path and Path(env_font_path).exists():
        try:
            fm.fontManager.addfont(str(env_font_path))
            prop = fm.FontProperties(fname=str(env_font_path))
            chosen = prop.get_name()
        except Exception:
            chosen = None

    # 2) 本地项目 fonts 目录
    if chosen is None:
        local_fonts_dir = PROJECT_ROOT / "fonts"
        if local_fonts_dir.exists():
            candidates = [
                "NotoSansCJK-Regular.ttc",
                "NotoSansSC-Regular.otf",
                "SourceHanSansSC-Regular.otf",
                "SimHei.ttf",
                "wqy-zenhei.ttc",
                "WenQuanYiZenHei.ttf",
                "msyh.ttf",
                "HarmonyOS_Sans_SC.ttf",
            ]
            for fname in candidates:
                fpath = local_fonts_dir / fname
                if fpath.exists():
                    try:
                        fm.fontManager.addfont(str(fpath))
                        prop = fm.FontProperties(fname=str(fpath))
                        chosen = prop.get_name()
                        break
                    except Exception:
                        continue

    # 3) 系统已安装字体中选取（不考虑 DejaVu Sans 等非 CJK 字体）
    if chosen is None:
        for name in preferred_fonts:
            if name in available_font_names:
                chosen = name
                break

    # 4) 扫描常见系统字体目录寻找匹配文件名
    def _scan_dir_for_cjk_font(dir_path):
        try:
            entries = list(Path(dir_path).rglob("*"))
        except Exception:
            return None
        name_patterns = [
            "NotoSansCJK",
            "NotoSansSC",
            "SourceHanSans",
            "WenQuanYi",
            "ZenHei",
            "SimHei",
            "msyh",
            "HarmonyOS_Sans",
        ]
        for p in entries:
            if p.is_file() and p.suffix.lower() in {".ttf", ".otf", ".ttc"}:
                lower = p.name.lower()
                if any(pattern.lower() in lower for pattern in name_patterns):
                    try:
                        fm.fontManager.addfont(str(p))
                        prop = fm.FontProperties(fname=str(p))
                        return prop.get_name()
                    except Exception:
                        pass
        return None

    if chosen is None:
        system_dirs = [
            "/usr/share/fonts",
            "/usr/local/share/fonts",
            str(Path.home() / ".fonts"),
            "/usr/share/fonts/opentype",
            "/usr/share/fonts/truetype",
        ]
        for d in system_dirs:
            chosen = _scan_dir_for_cjk_font(d)
            if chosen:
                break

    # 5) 尝试在线下载 NotoSansSC-Regular.otf 到项目 fonts/ 并使用
    if chosen is None:
        try:
            import urllib.request
            local_fonts_dir = PROJECT_ROOT / "fonts"
            local_fonts_dir.mkdir(exist_ok=True)
            target = local_fonts_dir / "NotoSansSC-Regular.otf"
            if not target.exists():
                url = "https://github.com/googlefonts/noto-cjk/raw/refs/heads/main/Sans/OTF/SimplifiedChinese/NotoSansSC-Regular.otf"
                urllib.request.urlretrieve(url, str(target))
            fm.fontManager.addfont(str(target))
            prop = fm.FontProperties(fname=str(target))
            chosen = prop.get_name()
        except Exception:
            chosen = None

    # 设置 rcParams（即使未找到也设置候选列表以便未来可用）
    font_list = [chosen] if chosen else []
    # 附加一组候选，保证有回退
    for name in preferred_fonts:
        if name not in font_list:
            font_list.append(name)

    # 将选择的中文字体作为首选；追加常见回退字体
    fallback = [
        "Noto Sans CJK SC",
        "Noto Sans CJK",
        "Noto Sans SC",
        "Source Han Sans SC",
        "Source Han Sans CN",
        "SimHei",
        "WenQuanYi Zen Hei",
        "WenQuanYi Micro Hei",
        "Microsoft YaHei",
        "Arial Unicode MS",
        "DejaVu Sans",
    ]
    full_list = font_list + [n for n in fallback if n not in font_list]
    matplotlib.rcParams["font.sans-serif"] = full_list
    matplotlib.rcParams["axes.unicode_minus"] = False
    return chosen


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
