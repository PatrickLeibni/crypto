#!/usr/bin/env python3
"""
数字水印系统总结演示
展示系统的主要功能和性能
"""

import numpy as np

# import cv2  
# import matplotlib.pyplot as plt 
from src.watermark import DigitalWatermark
from src.robustness_test import RobustnessTester


def print_summary():
    """打印系统总结"""
    print("=" * 60)
    print("基于数字水印的图片泄露检测系统")
    print("=" * 60)

    print("\n📁 生成文件:")
    print("• sample_image.jpg - 原始测试图片")
    print("• watermarked_image.jpg - 含水印图片")
    print("• watermark_comparison.png - 对比图")
    print("• robustness_report.txt - 详细测试报告")
    print("• robustness_plot.png - 测试结果图表")


def demonstrate_key_features():
    """演示关键功能"""
    print("\n" + "=" * 60)
    print("关键功能演示")
    print("=" * 60)

    # 初始化系统
    watermark_system = DigitalWatermark(alpha=0.15)

    # 测试水印嵌入
    print("\n1. 水印嵌入测试")
    original_img, watermarked_img = watermark_system.embed_watermark(
        "sample_image.jpg",
        message="Digital Watermark Test 2024",
        output_path="test_watermarked.jpg",
    )

    # 计算PSNR
    mse = np.mean(
        (original_img.astype(np.float64) - watermarked_img.astype(np.float64)) ** 2
    )
    psnr = 20 * np.log10(255.0 / np.sqrt(mse))
    print(f"✓ 水印嵌入成功")
    print(f"✓ PSNR: {psnr:.2f} dB")

    # 测试水印检测
    print("\n2. 水印检测测试")
    h, w = original_img.shape
    watermark_size = (h // 8) * (w // 8)

    detected, similarity = watermark_system.detect_watermark(
        "test_watermarked.jpg",
        original_size=(h, w),
        watermark_size=watermark_size,
        expected_message="Digital Watermark Test 2024",
    )

    print(f"✓ 水印检测: {'成功' if detected else '失败'}")
    print(f"✓ 相似度: {similarity:.4f}")

    # 测试鲁棒性
    print("\n3. 鲁棒性测试")
    tester = RobustnessTester(watermark_system)

    # 测试几种关键攻击
    attacks = [
        ("噪声攻击 (0.05)", lambda: tester.apply_noise("test_watermarked.jpg", 0.05)),
        (
            "压缩攻击 (90%)",
            lambda: tester.apply_compression("test_watermarked.jpg", 90),
        ),
        ("翻转攻击 (水平)", lambda: tester.apply_flip("test_watermarked.jpg", 1)),
        ("裁剪攻击 (90%)", lambda: tester.apply_crop("test_watermarked.jpg", 0.9)),
    ]

    for attack_name, attack_func in attacks:
        try:
            attacked_path = attack_func()
            detected, similarity = watermark_system.detect_watermark(
                attacked_path, (h, w), watermark_size, "Digital Watermark Test 2024"
            )
            status = "✓" if detected else "✗"
            print(f"{attack_name:20} | {status} | 相似度: {similarity:.4f}")

            # 清理临时文件
            import os

            if os.path.exists(attacked_path):
                os.remove(attacked_path)
        except Exception as e:
            print(f"{attack_name:20} | ✗ | 错误: {e}")


def show_performance_metrics():
    """显示性能指标"""
    print("\n" + "=" * 60)
    print("性能指标")
    print("=" * 60)

    # 读取测试报告
    try:
        with open("robustness_report.txt", "r", encoding="utf-8") as f:
            report = f.read()

        # 提取关键指标
        lines = report.split("\n")
        for line in lines:
            if "总测试数:" in line:
                print(f" {line}")
            elif "检测成功数:" in line:
                print(f" {line}")
            elif "检测成功率:" in line:
                print(f" {line}")
            elif "平均相似度:" in line:
                print(f" {line}")
            elif "攻击类型分析:" in line:
                print(f"\n {line}")
                break

        # 显示攻击类型分析
        in_analysis = False
        for line in lines:
            if "攻击类型分析:" in line:
                in_analysis = True
                continue
            if in_analysis and "|" in line and "成功率:" in line:
                print(f" {line}")
            elif in_analysis and line.strip() == "":
                break

    except FileNotFoundError:
        print("⚠️  测试报告文件未找到，请先运行 demo.py")


def main():
    """主函数"""
    print_summary()
    demonstrate_key_features()
    show_performance_metrics()

    print("\n" + "=" * 60)
    print(" 系统演示完成!")
    print("=" * 60)


if __name__ == "__main__":
    main()
