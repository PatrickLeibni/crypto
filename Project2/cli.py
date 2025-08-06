#!/usr/bin/env python3
"""
数字水印系统命令行界面
"""

import argparse
import sys
from pathlib import Path
import logging

from src.watermark import DigitalWatermark
from src.robustness_test import RobustnessTester
from utils import setup_logging, create_test_image, calculate_psnr, get_image_info
from config import WATERMARK_CONFIG


def embed_watermark(args):
    """嵌入水印"""
    logger = logging.getLogger(__name__)

    # 检查输入文件
    input_path = Path(args.input)
    if not input_path.exists():
        logger.error(f"输入文件不存在: {input_path}")
        return 1

    # 初始化水印系统
    watermark_system = DigitalWatermark(alpha=args.alpha)

    # 嵌入水印
    try:
        original_img, watermarked_img = watermark_system.embed_watermark(
            str(input_path),
            message=args.message,
            output_path=args.output,
            seed=args.seed,
        )

        # 计算PSNR
        psnr = calculate_psnr(original_img, watermarked_img)

        logger.info("水印嵌入成功!")
        logger.info(f"输入文件: {input_path}")
        logger.info(f"输出文件: {args.output}")
        logger.info(f"水印消息: {args.message}")
        logger.info(f"PSNR: {psnr:.2f} dB")

        return 0

    except Exception as e:
        logger.error(f"水印嵌入失败: {e}")
        return 1


def extract_watermark(args):
    """提取水印"""
    logger = logging.getLogger(__name__)

    # 检查输入文件
    input_path = Path(args.input)
    if not input_path.exists():
        logger.error(f"输入文件不存在: {input_path}")
        return 1

    # 初始化水印系统
    watermark_system = DigitalWatermark(alpha=args.alpha)

    # 获取图片信息
    img_info = get_image_info(input_path)
    if not img_info:
        logger.error(f"无法读取图片: {input_path}")
        return 1

    # 计算水印大小
    h, w = img_info["shape"][:2]
    watermark_size = (h // 8) * (w // 8)

    try:
        # 提取水印
        extracted_watermark = watermark_system.extract_watermark(
            str(input_path),
            original_size=(h, w),
            watermark_size=watermark_size,
            seed=args.seed,
        )

        logger.info("水印提取成功!")
        logger.info(f"图片尺寸: {w} x {h}")
        logger.info(f"水印大小: {watermark_size}")
        logger.info(f"提取的水印长度: {len(extracted_watermark)}")
        logger.info(
            f"水印中1的比例: {extracted_watermark.sum() / len(extracted_watermark):.4f}"
        )

        return 0

    except Exception as e:
        logger.error(f"水印提取失败: {e}")
        return 1


def detect_watermark(args):
    """检测水印"""
    logger = logging.getLogger(__name__)

    # 检查输入文件
    input_path = Path(args.input)
    if not input_path.exists():
        logger.error(f"输入文件不存在: {input_path}")
        return 1

    # 初始化水印系统
    watermark_system = DigitalWatermark(alpha=args.alpha)

    # 获取图片信息
    img_info = get_image_info(input_path)
    if not img_info:
        logger.error(f"无法读取图片: {input_path}")
        return 1

    # 计算水印大小
    h, w = img_info["shape"][:2]
    watermark_size = (h // 8) * (w // 8)

    try:
        # 检测水印
        detected, similarity = watermark_system.detect_watermark(
            str(input_path),
            original_size=(h, w),
            watermark_size=watermark_size,
            expected_message=args.message,
            threshold=args.threshold,
            seed=args.seed,
        )

        logger.info("水印检测完成!")
        logger.info(f"检测结果: {'成功' if detected else '失败'}")
        logger.info(f"相似度: {similarity:.4f}")
        logger.info(f"阈值: {args.threshold}")

        return 0 if detected else 1

    except Exception as e:
        logger.error(f"水印检测失败: {e}")
        return 1


def test_robustness(args):
    """鲁棒性测试"""
    logger = logging.getLogger(__name__)

    # 检查输入文件
    input_path = Path(args.input)
    if not input_path.exists():
        logger.error(f"输入文件不存在: {input_path}")
        return 1

    # 初始化系统
    watermark_system = DigitalWatermark(alpha=args.alpha)
    tester = RobustnessTester(watermark_system)

    # 获取图片信息
    img_info = get_image_info(input_path)
    if not img_info:
        logger.error(f"无法读取图片: {input_path}")
        return 1

    # 计算水印大小
    h, w = img_info["shape"][:2]
    watermark_size = (h // 8) * (w // 8)

    try:
        # 运行鲁棒性测试
        results = tester.run_comprehensive_test(
            str(input_path),
            str(input_path),  # 使用同一文件作为原始和含水印图片
            original_size=(h, w),
            watermark_size=watermark_size,
            expected_message=args.message,
        )

        # 生成报告
        if args.report:
            tester.generate_report(args.report)
            logger.info(f"测试报告已保存到: {args.report}")

        if args.plot:
            tester.plot_results(args.plot)
            logger.info(f"测试图表已保存到: {args.plot}")

        # 显示统计信息
        total_tests = len(results)
        detected_count = sum(1 for result in results.values() if result["detected"])
        avg_similarity = (
            sum(result["similarity"] for result in results.values()) / total_tests
        )

        logger.info("鲁棒性测试完成!")
        logger.info(f"总测试数: {total_tests}")
        logger.info(f"检测成功数: {detected_count}")
        logger.info(f"检测成功率: {detected_count/total_tests*100:.2f}%")
        logger.info(f"平均相似度: {avg_similarity:.4f}")

        return 0

    except Exception as e:
        logger.error(f"鲁棒性测试失败: {e}")
        return 1


def create_sample(args):
    """创建示例图片"""
    logger = logging.getLogger(__name__)

    output_path = Path(args.output)

    try:
        create_test_image(output_path, size=(args.width, args.height))
        logger.info(f"示例图片已创建: {output_path}")
        return 0

    except Exception as e:
        logger.error(f"创建示例图片失败: {e}")
        return 1


def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description="数字水印系统命令行工具",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例用法:
  %(prog)s embed -i input.jpg -o output.jpg -m "My watermark"
  %(prog)s extract -i watermarked.jpg
  %(prog)s detect -i watermarked.jpg -m "My watermark"
  %(prog)s test -i watermarked.jpg -m "My watermark" --report report.txt
  %(prog)s create-sample -o sample.jpg
        """,
    )

    # 设置日志
    setup_logging()

    subparsers = parser.add_subparsers(dest="command", help="可用命令")

    # 嵌入水印命令
    embed_parser = subparsers.add_parser("embed", help="嵌入水印")
    embed_parser.add_argument("-i", "--input", required=True, help="输入图片路径")
    embed_parser.add_argument("-o", "--output", required=True, help="输出图片路径")
    embed_parser.add_argument(
        "-m", "--message", default="Digital Watermark", help="水印消息"
    )
    embed_parser.add_argument(
        "-a",
        "--alpha",
        type=float,
        default=WATERMARK_CONFIG["default_alpha"],
        help="水印强度",
    )
    embed_parser.add_argument(
        "-s",
        "--seed",
        type=int,
        default=WATERMARK_CONFIG["default_seed"],
        help="随机种子",
    )
    embed_parser.set_defaults(func=embed_watermark)

    # 提取水印命令
    extract_parser = subparsers.add_parser("extract", help="提取水印")
    extract_parser.add_argument("-i", "--input", required=True, help="输入图片路径")
    extract_parser.add_argument(
        "-a",
        "--alpha",
        type=float,
        default=WATERMARK_CONFIG["default_alpha"],
        help="水印强度",
    )
    extract_parser.add_argument(
        "-s",
        "--seed",
        type=int,
        default=WATERMARK_CONFIG["default_seed"],
        help="随机种子",
    )
    extract_parser.set_defaults(func=extract_watermark)

    # 检测水印命令
    detect_parser = subparsers.add_parser("detect", help="检测水印")
    detect_parser.add_argument("-i", "--input", required=True, help="输入图片路径")
    detect_parser.add_argument("-m", "--message", help="期望的水印消息")
    detect_parser.add_argument(
        "-t",
        "--threshold",
        type=float,
        default=WATERMARK_CONFIG["default_threshold"],
        help="检测阈值",
    )
    detect_parser.add_argument(
        "-a",
        "--alpha",
        type=float,
        default=WATERMARK_CONFIG["default_alpha"],
        help="水印强度",
    )
    detect_parser.add_argument(
        "-s",
        "--seed",
        type=int,
        default=WATERMARK_CONFIG["default_seed"],
        help="随机种子",
    )
    detect_parser.set_defaults(func=detect_watermark)

    # 鲁棒性测试命令
    test_parser = subparsers.add_parser("test", help="鲁棒性测试")
    test_parser.add_argument("-i", "--input", required=True, help="输入图片路径")
    test_parser.add_argument("-m", "--message", help="期望的水印消息")
    test_parser.add_argument("--report", help="测试报告输出路径")
    test_parser.add_argument("--plot", help="测试图表输出路径")
    test_parser.add_argument(
        "-a",
        "--alpha",
        type=float,
        default=WATERMARK_CONFIG["default_alpha"],
        help="水印强度",
    )
    test_parser.set_defaults(func=test_robustness)

    # 创建示例图片命令
    sample_parser = subparsers.add_parser("create-sample", help="创建示例图片")
    sample_parser.add_argument("-o", "--output", required=True, help="输出图片路径")
    sample_parser.add_argument("--width", type=int, default=512, help="图片宽度")
    sample_parser.add_argument("--height", type=int, default=512, help="图片高度")
    sample_parser.set_defaults(func=create_sample)

    # 解析参数
    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        return 1

    # 执行命令
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
