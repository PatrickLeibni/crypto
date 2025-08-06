#!/usr/bin/env python3
"""
数字水印系统演示脚本
基于DCT变换的图片水印嵌入和提取，包含鲁棒性测试
"""

import cv2
import numpy as np
import matplotlib.pyplot as plt
import sys
import os

# 添加项目根目录到Python路径
sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

try:
    from src.watermark import DigitalWatermark
    from src.robustness_test import RobustnessTester
except ImportError:
    print("错误: 无法导入水印模块")
    print("请确保在项目根目录下运行此脚本")
    sys.exit(1)


def create_sample_image(
    output_path: str = "output/sample_image.jpg", size: tuple = (512, 512)
):
    """
    创建示例图片用于测试

    Args:
        output_path: 输出路径
        size: 图片尺寸 (width, height)
    """
    # 创建一个渐变图片
    img = np.zeros((size[1], size[0], 3), dtype=np.uint8)

    # 创建渐变效果
    for i in range(size[1]):
        for j in range(size[0]):
            img[i, j] = [
                int(255 * i / size[1]),  # 红色渐变
                int(255 * j / size[0]),  # 绿色渐变
                int(255 * (i + j) / (size[0] + size[1])),  # 蓝色渐变
            ]

    # 添加一些几何图形
    cv2.circle(img, (size[0] // 4, size[1] // 4), 50, (255, 255, 255), -1)
    cv2.rectangle(
        img,
        (size[0] // 2, size[1] // 2),
        (size[0] // 2 + 100, size[1] // 2 + 100),
        (0, 255, 0),
        3,
    )
    cv2.line(img, (0, 0), (size[0], size[1]), (255, 0, 255), 5)

    cv2.imwrite(output_path, img)
    print("示例图片已创建:", output_path)
    return output_path


def demonstrate_watermarking():
    """
    演示水印嵌入和提取功能
    """
    print("=" * 60)
    print("数字水印系统演示")
    print("=" * 60)

    # 创建示例图片
    sample_image = create_sample_image()

    # 初始化水印系统
    watermark_system = DigitalWatermark(alpha=0.15)

    # 水印消息
    watermark_message = "Digital Watermark Test 2024"

    print("\n1. 水印嵌入演示")
    print("原始图片:", sample_image)
    print("水印消息:", "'" + watermark_message + "'")

    # 嵌入水印
    original_img, watermarked_img = watermark_system.embed_watermark(
        sample_image,
        message=watermark_message,
        output_path="output/watermarked_image.jpg",
    )

    print("水印嵌入完成!")
    print("含水印图片已保存为: output/watermarked_image.jpg")

    # 计算图片尺寸和水印大小
    h, w = original_img.shape
    watermark_size = (h // 8) * (w // 8)

    print("\n2. 水印检测演示")
    print("图片尺寸:", f"{w} x {h}")
    print("水印大小:", watermark_size)

    # 检测水印
    detected, similarity = watermark_system.detect_watermark(
        "output/watermarked_image.jpg",
        original_size=(h, w),
        watermark_size=watermark_size,
        expected_message=watermark_message,
    )

    print("水印检测结果:", "成功" if detected else "失败")
    print("相似度:", f"{similarity:.4f}")

    # 提取水印
    extracted_watermark = watermark_system.extract_watermark(
        "output/watermarked_image.jpg",
        original_size=(h, w),
        watermark_size=watermark_size,
    )

    print("提取的水印长度:", len(extracted_watermark))
    ratio = np.sum(extracted_watermark) / len(extracted_watermark)
    print("水印中1的比例:", f"{ratio:.4f}")

    return (
        sample_image,
        "output/watermarked_image.jpg",
        (h, w),
        watermark_size,
        watermark_message,
    )


def demonstrate_robustness_testing(
    sample_image: str,
    watermarked_image: str,
    original_size: tuple,
    watermark_size: int,
    watermark_message: str,
):
    """
    演示鲁棒性测试
    """
    print("\n3. 鲁棒性测试演示")
    print("=" * 40)

    # 初始化测试器
    watermark_system = DigitalWatermark(alpha=0.15)
    tester = RobustnessTester(watermark_system)

    # 运行全面测试
    results = tester.run_comprehensive_test(
        sample_image,
        watermarked_image,
        original_size,
        watermark_size,
        watermark_message,
    )

    # 生成报告
    tester.generate_report("output/robustness_report.txt")
    tester.plot_results("output/robustness_plot.png")

    # 显示部分结果
    print("\n部分测试结果:")
    print("-" * 30)
    for test_name, result in list(results.items())[:10]:
        status = "✓" if result["detected"] else "✗"
        print(f"{test_name:20} | {status} | 相似度: {result['similarity']:.4f}")

    # 统计信息
    total_tests = len(results)
    detected_count = sum(1 for result in results.values() if result["detected"])
    avg_similarity = np.mean([result["similarity"] for result in results.values()])

    print("\n总体统计:")
    print(f"总测试数: {total_tests}")
    print(f"检测成功数: {detected_count}")
    print(f"检测成功率: {detected_count/total_tests*100:.2f}%")
    print("平均相似度:", f"{avg_similarity:.4f}")


def demonstrate_individual_attacks(
    sample_image: str,
    watermarked_image: str,
    original_size: tuple,
    watermark_size: int,
    watermark_message: str,
):
    """
    演示个别攻击类型
    """
    print("\n4. 个别攻击演示")
    print("=" * 30)

    watermark_system = DigitalWatermark(alpha=0.15)
    tester = RobustnessTester(watermark_system)

    # 测试旋转攻击
    print("\n旋转攻击测试:")
    for angle in [5, 15, 30]:
        rotated_path = tester.apply_rotation(watermarked_image, angle)
        detected, similarity = watermark_system.detect_watermark(
            rotated_path, original_size, watermark_size, watermark_message
        )
        print(
            f"旋转 {angle}°: {'检测成功' if detected else '检测失败'} (相似度: {similarity:.4f})"
        )
        os.remove(rotated_path)

    # 测试噪声攻击
    print("\n噪声攻击测试:")
    for noise_level in [0.05, 0.1, 0.15]:
        noisy_path = tester.apply_noise(watermarked_image, noise_level)
        detected, similarity = watermark_system.detect_watermark(
            noisy_path, original_size, watermark_size, watermark_message
        )
        print(
            f"噪声 {noise_level}: {'检测成功' if detected else '检测失败'} (相似度: {similarity:.4f})"
        )
        os.remove(noisy_path)

    # 测试压缩攻击
    print("\n压缩攻击测试:")
    for quality in [90, 70, 50]:
        compressed_path = tester.apply_compression(watermarked_image, quality)
        detected, similarity = watermark_system.detect_watermark(
            compressed_path, original_size, watermark_size, watermark_message
        )
        print(
            f"压缩质量 {quality}: {'检测成功' if detected else '检测失败'} (相似度: {similarity:.4f})"
        )
        os.remove(compressed_path)

    # 测试平移攻击
    print("\n平移攻击测试:")
    for dx, dy in [(10, 10), (20, 20), (30, 30)]:
        translated_path = tester.apply_translation(watermarked_image, dx, dy)
        detected, similarity = watermark_system.detect_watermark(
            translated_path, original_size, watermark_size, watermark_message
        )
        print(
            f"平移 ({dx}, {dy}): {'检测成功' if detected else '检测失败'} (相似度: {similarity:.4f})"
        )
        os.remove(translated_path)


def visualize_results():
    """
    可视化结果
    """
    print("\n5. 结果可视化")
    print("=" * 20)

    # 读取图片
    original = cv2.imread("output/sample_image.jpg")
    watermarked = cv2.imread("output/watermarked_image.jpg")

    if original is not None and watermarked is not None:
        # 转换为RGB
        original_rgb = cv2.cvtColor(original, cv2.COLOR_BGR2RGB)
        watermarked_rgb = cv2.cvtColor(watermarked, cv2.COLOR_BGR2RGB)

        # 计算差异
        diff = cv2.absdiff(original, watermarked)
        diff_rgb = cv2.cvtColor(diff, cv2.COLOR_BGR2RGB)

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

        plt.tight_layout()
        plt.savefig("output/watermark_comparison.png", dpi=300, bbox_inches="tight")
        plt.close()

        print("对比图已保存为: output/watermark_comparison.png")

        # 计算PSNR
        mse = np.mean(
            (original.astype(np.float64) - watermarked.astype(np.float64)) ** 2
        )
        if mse > 0:
            psnr = 20 * np.log10(255.0 / np.sqrt(mse))
            print(f"PSNR: {psnr:.2f} dB")


def main():
    """
    主函数
    """
    print("数字水印系统 - 基于DCT变换的图片水印嵌入和提取")
    print("=" * 60)

    try:
        # 1. 水印嵌入和提取演示
        (
            sample_image,
            watermarked_image,
            original_size,
            watermark_size,
            watermark_message,
        ) = demonstrate_watermarking()

        # 2. 鲁棒性测试演示
        demonstrate_robustness_testing(
            sample_image,
            watermarked_image,
            original_size,
            watermark_size,
            watermark_message,
        )

        # 3. 个别攻击演示
        demonstrate_individual_attacks(
            sample_image,
            watermarked_image,
            original_size,
            watermark_size,
            watermark_message,
        )

        # 4. 结果可视化
        visualize_results()

        print("\n演示完成!")
        print("生成的文件:")
        print("- output/sample_image.jpg (原始图片)")
        print("- output/watermarked_image.jpg (含水印图片)")
        print("- output/watermark_comparison.png (对比图)")
        print("- output/robustness_report.txt (测试报告)")
        print("- output/robustness_plot.png (测试结果图表)")

    except Exception as e:
        print(f"演示过程中出现错误: {e}")
        import traceback

        traceback.print_exc()


if __name__ == "__main__":
    main()
