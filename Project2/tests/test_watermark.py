#!/usr/bin/env python3
"""
数字水印系统测试套件
"""

import sys
import unittest
import tempfile
import shutil
from pathlib import Path
import numpy as np

# import cv2  # 未使用，注释掉

sys.path.insert(0, str(Path(__file__).parent.parent))

from src.watermark import DigitalWatermark
from src.robustness_test import RobustnessTester
from utils import create_test_image, calculate_psnr, get_image_info


class TestDigitalWatermark(unittest.TestCase):
    """数字水印系统测试类"""

    def setUp(self):
        """测试前准备"""
        self.temp_dir = Path(tempfile.mkdtemp())
        self.test_image_path = self.temp_dir / "test_image.jpg"
        self.watermark_system = DigitalWatermark(alpha=0.15)

        # 创建测试图片
        create_test_image(self.test_image_path, size=(256, 256))

    def tearDown(self):
        """测试后清理"""
        shutil.rmtree(self.temp_dir)

    def test_watermark_embedding(self):
        """测试水印嵌入"""
        # 嵌入水印
        original_img, watermarked_img = self.watermark_system.embed_watermark(
            str(self.test_image_path),
            message="Test watermark",
            output_path=str(self.temp_dir / "watermarked.jpg"),
        )

        # 验证结果
        self.assertIsNotNone(original_img)
        self.assertIsNotNone(watermarked_img)
        self.assertEqual(original_img.shape, watermarked_img.shape)

        # 验证PSNR
        psnr = calculate_psnr(original_img, watermarked_img)
        self.assertGreater(psnr, 30)  # PSNR应该大于30dB

    def test_watermark_extraction(self):
        """测试水印提取"""
        # 嵌入水印
        original_img, watermarked_img = self.watermark_system.embed_watermark(
            str(self.test_image_path),
            message="Test watermark",
            output_path=str(self.temp_dir / "watermarked.jpg"),
        )

        # 计算水印大小
        h, w = original_img.shape
        watermark_size = (h // 8) * (w // 8)

        # 提取水印
        extracted_watermark = self.watermark_system.extract_watermark(
            str(self.temp_dir / "watermarked.jpg"),
            original_size=(h, w),
            watermark_size=watermark_size,
        )

        # 验证结果
        self.assertIsNotNone(extracted_watermark)
        self.assertEqual(len(extracted_watermark), watermark_size)
        self.assertTrue(all(bit in [0, 1] for bit in extracted_watermark))

    def test_watermark_detection(self):
        """测试水印检测"""
        # 嵌入水印
        original_img, watermarked_img = self.watermark_system.embed_watermark(
            str(self.test_image_path),
            message="Test watermark",
            output_path=str(self.temp_dir / "watermarked.jpg"),
        )

        # 计算水印大小
        h, w = original_img.shape
        watermark_size = (h // 8) * (w // 8)

        # 检测水印
        detected, similarity = self.watermark_system.detect_watermark(
            str(self.temp_dir / "watermarked.jpg"),
            original_size=(h, w),
            watermark_size=watermark_size,
            expected_message="Test watermark",
        )

        # 验证结果
        self.assertTrue(detected)
        self.assertGreater(similarity, 0.6)

    def test_different_messages(self):
        """测试不同消息的水印"""
        messages = ["Message 1", "Message 2", "Test watermark"]

        for message in messages:
            with self.subTest(message=message):
                # 嵌入水印
                original_img, watermarked_img = self.watermark_system.embed_watermark(
                    str(self.test_image_path),
                    message=message,
                    output_path=str(self.temp_dir / "watermarked.jpg"),
                )

                # 计算水印大小
                h, w = original_img.shape
                watermark_size = (h // 8) * (w // 8)

                # 检测水印
                detected, similarity = self.watermark_system.detect_watermark(
                    str(self.temp_dir / "watermarked.jpg"),
                    original_size=(h, w),
                    watermark_size=watermark_size,
                    expected_message=message,
                )

                # 验证结果
                self.assertTrue(detected)
                self.assertGreater(similarity, 0.6)

    def test_alpha_parameter(self):
        """测试alpha参数的影响"""
        alphas = [0.05, 0.1, 0.15, 0.2]

        for alpha in alphas:
            with self.subTest(alpha=alpha):
                watermark_system = DigitalWatermark(alpha=alpha)

                # 嵌入水印
                original_img, watermarked_img = watermark_system.embed_watermark(
                    str(self.test_image_path), message="Test watermark"
                )

                # 计算PSNR
                psnr = calculate_psnr(original_img, watermarked_img)

                # 验证PSNR随alpha变化
                if alpha > 0.1:
                    self.assertLess(psnr, 60)  # 调整PSNR阈值，高alpha值会降低PSNR
                else:
                    self.assertGreater(psnr, 40)  # 低alpha值会提高PSNR


class TestRobustnessTester(unittest.TestCase):
    """鲁棒性测试器测试类"""

    def setUp(self):
        """测试前准备"""
        self.temp_dir = Path(tempfile.mkdtemp())
        self.test_image_path = self.temp_dir / "test_image.jpg"
        self.watermark_system = DigitalWatermark(alpha=0.15)
        self.tester = RobustnessTester(self.watermark_system)

        # 创建测试图片
        create_test_image(self.test_image_path, size=(256, 256))

    def tearDown(self):
        """测试后清理"""
        shutil.rmtree(self.temp_dir)

    def test_rotation_attack(self):
        """测试旋转攻击"""
        # 嵌入水印
        original_img, watermarked_img = self.watermark_system.embed_watermark(
            str(self.test_image_path),
            message="Test watermark",
            output_path=str(self.temp_dir / "watermarked.jpg"),
        )

        # 应用旋转攻击
        rotated_path = self.tester.apply_rotation(
            str(self.temp_dir / "watermarked.jpg"), angle=15
        )

        # 验证结果
        self.assertTrue(Path(rotated_path).exists())

        # 清理
        Path(rotated_path).unlink()

    def test_noise_attack(self):
        """测试噪声攻击"""
        # 嵌入水印
        original_img, watermarked_img = self.watermark_system.embed_watermark(
            str(self.test_image_path),
            message="Test watermark",
            output_path=str(self.temp_dir / "watermarked.jpg"),
        )

        # 应用噪声攻击
        noisy_path = self.tester.apply_noise(
            str(self.temp_dir / "watermarked.jpg"), noise_level=0.1
        )

        # 验证结果
        self.assertTrue(Path(noisy_path).exists())

        # 清理
        Path(noisy_path).unlink()

    def test_compression_attack(self):
        """测试压缩攻击"""
        # 嵌入水印
        original_img, watermarked_img = self.watermark_system.embed_watermark(
            str(self.test_image_path),
            message="Test watermark",
            output_path=str(self.temp_dir / "watermarked.jpg"),
        )

        # 应用压缩攻击
        compressed_path = self.tester.apply_compression(
            str(self.temp_dir / "watermarked.jpg"), quality=80
        )

        # 验证结果
        self.assertTrue(Path(compressed_path).exists())

        # 清理
        Path(compressed_path).unlink()


class TestUtils(unittest.TestCase):
    """工具函数测试类"""

    def setUp(self):
        """测试前准备"""
        self.temp_dir = Path(tempfile.mkdtemp())

    def tearDown(self):
        """测试后清理"""
        shutil.rmtree(self.temp_dir)

    def test_create_test_image(self):
        """测试创建测试图片"""
        output_path = self.temp_dir / "test.jpg"

        # 创建测试图片
        result_path = create_test_image(output_path, size=(128, 128))

        # 验证结果
        self.assertTrue(result_path.exists())

        # 验证图片信息
        img_info = get_image_info(result_path)
        self.assertEqual(img_info["shape"], (128, 128, 3))

    def test_calculate_psnr(self):
        """测试PSNR计算"""
        # 创建测试图片
        original = np.random.randint(0, 256, (100, 100, 3), dtype=np.uint8)
        processed = original.copy()

        # 计算PSNR
        psnr = calculate_psnr(original, processed)

        # 验证结果
        self.assertEqual(psnr, float("inf"))  # 相同图片的PSNR应该是无穷大

        # 测试有噪声的图片
        noisy = original + np.random.normal(0, 10, original.shape).astype(np.uint8)
        noisy = np.clip(noisy, 0, 255)

        psnr_noisy = calculate_psnr(original, noisy)
        self.assertLess(psnr_noisy, float("inf"))
        self.assertGreater(psnr_noisy, 0)

    def test_get_image_info(self):
        """测试获取图片信息"""
        # 创建测试图片
        test_image_path = self.temp_dir / "test.jpg"
        create_test_image(test_image_path, size=(64, 64))

        # 获取图片信息
        img_info = get_image_info(test_image_path)

        # 验证结果
        self.assertIsNotNone(img_info)
        self.assertEqual(img_info["shape"], (64, 64, 3))
        self.assertEqual(img_info["channels"], 3)
        self.assertGreater(img_info["file_size"], 0)


def run_tests():
    """运行所有测试"""
    # 创建测试套件
    test_suite = unittest.TestSuite()

    # 添加测试类
    test_classes = [TestDigitalWatermark, TestRobustnessTester, TestUtils]

    for test_class in test_classes:
        tests = unittest.TestLoader().loadTestsFromTestCase(test_class)
        test_suite.addTests(tests)

    # 运行测试
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(test_suite)

    return result.wasSuccessful()


if __name__ == "__main__":
    success = run_tests()
    exit(0 if success else 1)
