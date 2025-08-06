#!/usr/bin/env python3
"""
安装测试脚本 - 验证所有依赖是否正确安装
"""


def test_imports():
    """测试所有必要的导入"""
    print("测试导入...")

    try:
        import cv2

        print("✓ OpenCV 导入成功")
    except ImportError as e:
        print(f"✗ OpenCV 导入失败: {e}")
        return False

    try:
        import numpy as np

        print("✓ NumPy 导入成功")
    except ImportError as e:
        print(f"✗ NumPy 导入失败: {e}")
        return False

    try:
        from PIL import Image, ImageEnhance, ImageFilter

        print("✓ Pillow 导入成功")
    except ImportError as e:
        print(f"✗ Pillow 导入失败: {e}")
        return False

    try:
        import matplotlib.pyplot as plt

        print("✓ Matplotlib 导入成功")
    except ImportError as e:
        print(f"✗ Matplotlib 导入失败: {e}")
        return False

    try:
        import hashlib
        import random

        print("✓ 标准库导入成功")
    except ImportError as e:
        print(f"✗ 标准库导入失败: {e}")
        return False

    return True


def test_watermark_system():
    """测试水印系统"""
    print("\n测试水印系统...")

    try:
        import sys
        from pathlib import Path

        sys.path.insert(0, str(Path(__file__).parent.parent))

        from src.watermark import DigitalWatermark

        print("✓ 水印系统导入成功")

        # 创建水印系统实例
        watermark_system = DigitalWatermark(alpha=0.1)
        print("✓ 水印系统初始化成功")
        # 使用变量避免未使用警告
        assert watermark_system is not None
        return True

    except Exception as e:
        print(f"✗ 水印系统测试失败: {e}")
        return False


def test_robustness_tester():
    """测试鲁棒性测试器"""
    print("\n测试鲁棒性测试器...")

    try:
        import sys
        from pathlib import Path

        sys.path.insert(0, str(Path(__file__).parent.parent))

        from src.robustness_test import RobustnessTester
        from src.watermark import DigitalWatermark

        watermark_system = DigitalWatermark(alpha=0.1)
        tester = RobustnessTester(watermark_system)
        print("✓ 鲁棒性测试器初始化成功")
        # 使用变量避免未使用警告
        assert tester is not None
        return True

    except Exception as e:
        print(f"✗ 鲁棒性测试器测试失败: {e}")
        return False


def test_basic_functionality():
    """测试基本功能"""
    print("\n测试基本功能...")

    try:
        import cv2
        import numpy as np

        # 创建测试图片
        test_img = np.random.randint(0, 255, (64, 64, 3), dtype=np.uint8)
        cv2.imwrite("test_image.jpg", test_img)
        print("✓ 图片创建和保存成功")

        # 读取图片
        loaded_img = cv2.imread("test_image.jpg")
        if loaded_img is not None:
            print("✓ 图片读取成功")
        else:
            print("✗ 图片读取失败")
            assert False, "图片读取失败"

        # 清理测试文件
        import os

        if os.path.exists("test_image.jpg"):
            os.remove("test_image.jpg")
            print("✓ 测试文件清理成功")

    except Exception as e:
        print(f"✗ 基本功能测试失败: {e}")
        return False

    return True


def main():
    """主测试函数"""
    print("数字水印系统 - 安装测试")
    print("=" * 40)

    all_tests_passed = True

    # 测试导入
    if not test_imports():
        all_tests_passed = False

    # 测试水印系统
    if not test_watermark_system():
        all_tests_passed = False

    # 测试鲁棒性测试器
    if not test_robustness_tester():
        all_tests_passed = False

    # 测试基本功能
    if not test_basic_functionality():
        all_tests_passed = False

    print("\n" + "=" * 40)
    if all_tests_passed:
        print("✓ 所有测试通过！系统安装成功。")
        print("现在可以运行 demo.py 来查看完整演示。")
    else:
        print("✗ 部分测试失败，请检查依赖安装。")
        print("请运行: pip install -r requirements.txt")

    return all_tests_passed


if __name__ == "__main__":
    main()
