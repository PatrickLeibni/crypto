import cv2
import numpy as np
from PIL import Image, ImageEnhance, ImageFilter
import matplotlib.pyplot as plt
from typing import Tuple, Dict
import os
from .watermark import DigitalWatermark
from utils import configure_chinese_fonts


class RobustnessTester:
    """
    水印鲁棒性测试器 - 测试各种攻击对水印的影响
    """

    def __init__(self, watermark_system: DigitalWatermark):
        """
        初始化测试器

        Args:
            watermark_system: 水印系统实例
        """
        self.watermark_system = watermark_system
        # 确保中文字体
        try:
            configure_chinese_fonts()
        except Exception:
            pass
        self.test_results = {}

    def apply_rotation(
        self, image_path: str, angle: float, output_path: str = None
    ) -> str:
        """
        应用旋转变换

        Args:
            image_path: 输入图片路径
            angle: 旋转角度 (度)
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        h, w = img.shape[:2]
        center = (w // 2, h // 2)

        # 计算旋转矩阵
        rotation_matrix = cv2.getRotationMatrix2D(center, angle, 1.0)

        # 应用旋转变换
        rotated = cv2.warpAffine(img, rotation_matrix, (w, h))

        if output_path:
            cv2.imwrite(output_path, rotated)
            return output_path
        else:
            temp_path = f"rotated_{angle}_{os.path.basename(image_path)}"
            cv2.imwrite(temp_path, rotated)
            return temp_path

    def apply_flip(
        self, image_path: str, flip_code: int, output_path: str = None
    ) -> str:
        """
        应用翻转变换

        Args:
            image_path: 输入图片路径
            flip_code: 翻转类型 (0=垂直, 1=水平, -1=垂直+水平)
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        flipped = cv2.flip(img, flip_code)

        if output_path:
            cv2.imwrite(output_path, flipped)
            return output_path
        else:
            temp_path = f"flipped_{flip_code}_{os.path.basename(image_path)}"
            cv2.imwrite(temp_path, flipped)
            return temp_path

    def apply_crop(
        self, image_path: str, crop_ratio: float, output_path: str = None
    ) -> str:
        """
        应用裁剪变换

        Args:
            image_path: 输入图片路径
            crop_ratio: 裁剪比例 (0.1-1.0)
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        h, w = img.shape[:2]
        new_h, new_w = int(h * crop_ratio), int(w * crop_ratio)

        # 从中心裁剪
        start_y = (h - new_h) // 2
        start_x = (w - new_w) // 2
        cropped = img[start_y : start_y + new_h, start_x : start_x + new_w]

        if output_path:
            cv2.imwrite(output_path, cropped)
            return output_path
        else:
            temp_path = f"cropped_{crop_ratio}_{os.path.basename(image_path)}"
            cv2.imwrite(temp_path, cropped)
            return temp_path

    def apply_noise(
        self, image_path: str, noise_level: float, output_path: str = None
    ) -> str:
        """
        添加噪声

        Args:
            image_path: 输入图片路径
            noise_level: 噪声强度 (0.01-0.5)
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        # 生成高斯噪声
        noise = np.random.normal(0, noise_level * 255, img.shape).astype(np.float64)
        noisy_img = img.astype(np.float64) + noise
        noisy_img = np.clip(noisy_img, 0, 255).astype(np.uint8)

        if output_path:
            cv2.imwrite(output_path, noisy_img)
            return output_path
        else:
            temp_path = f"noisy_{noise_level}_{os.path.basename(image_path)}"
            cv2.imwrite(temp_path, noisy_img)
            return temp_path

    def apply_compression(
        self, image_path: str, quality: int, output_path: str = None
    ) -> str:
        """
        应用JPEG压缩

        Args:
            image_path: 输入图片路径
            quality: 压缩质量 (1-100)
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = Image.open(image_path)

        if output_path:
            img.save(output_path, "JPEG", quality=quality)
            return output_path
        else:
            temp_path = f"compressed_{quality}_{os.path.basename(image_path)}"
            img.save(temp_path, "JPEG", quality=quality)
            return temp_path

    def apply_brightness_contrast(
        self,
        image_path: str,
        brightness: float,
        contrast: float,
        output_path: str = None,
    ) -> str:
        """
        调整亮度和对比度

        Args:
            image_path: 输入图片路径
            brightness: 亮度调整 (0.1-3.0)
            contrast: 对比度调整 (0.1-3.0)
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = Image.open(image_path)

        # 调整亮度
        enhancer = ImageEnhance.Brightness(img)
        img = enhancer.enhance(brightness)

        # 调整对比度
        enhancer = ImageEnhance.Contrast(img)
        img = enhancer.enhance(contrast)

        if output_path:
            img.save(output_path)
            return output_path
        else:
            temp_path = (
                f"adjusted_b{brightness}_c{contrast}_{os.path.basename(image_path)}"
            )
            img.save(temp_path)
            return temp_path

    def apply_blur(
        self, image_path: str, blur_radius: float, output_path: str = None
    ) -> str:
        """
        应用模糊效果

        Args:
            image_path: 输入图片路径
            blur_radius: 模糊半径
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = Image.open(image_path)
        blurred = img.filter(ImageFilter.GaussianBlur(radius=blur_radius))

        if output_path:
            blurred.save(output_path)
            return output_path
        else:
            temp_path = f"blurred_{blur_radius}_{os.path.basename(image_path)}"
            blurred.save(temp_path)
            return temp_path

    def apply_scaling(
        self, image_path: str, scale_factor: float, output_path: str = None
    ) -> str:
        """
        应用缩放变换

        Args:
            image_path: 输入图片路径
            scale_factor: 缩放因子
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        h, w = img.shape[:2]
        new_h, new_w = int(h * scale_factor), int(w * scale_factor)

        scaled = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_LINEAR)

        if output_path:
            cv2.imwrite(output_path, scaled)
            return output_path
        else:
            temp_path = f"scaled_{scale_factor}_{os.path.basename(image_path)}"
            cv2.imwrite(temp_path, scaled)
            return temp_path

    def apply_translation(
        self, image_path: str, dx: int, dy: int, output_path: str = None
    ) -> str:
        """
        应用平移变换

        Args:
            image_path: 输入图片路径
            dx: X方向平移像素
            dy: Y方向平移像素
            output_path: 输出路径

        Returns:
            处理后的图片路径
        """
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        h, w = img.shape[:2]

        # 创建平移矩阵
        translation_matrix = np.float32([[1, 0, dx], [0, 1, dy]])

        # 应用平移变换
        translated = cv2.warpAffine(img, translation_matrix, (w, h))

        if output_path:
            cv2.imwrite(output_path, translated)
            return output_path
        else:
            temp_path = f"translated_{dx}_{dy}_{os.path.basename(image_path)}"
            cv2.imwrite(temp_path, translated)
            return temp_path

    def run_comprehensive_test(
        self,
        original_image: str,
        watermarked_image: str,
        original_size: Tuple[int, int],
        watermark_size: int,
        expected_message: str = None,
    ) -> Dict[str, Dict]:
        """
        运行全面的鲁棒性测试

        Args:
            original_image: 原始图片路径
            watermarked_image: 含水印图片路径
            original_size: 原始图片尺寸
            watermark_size: 水印大小
            expected_message: 期望的水印消息

        Returns:
            测试结果字典
        """
        results = {}

        # 1. 旋转测试
        print("测试旋转攻击...")
        for angle in [5, 10, 15, 30, 45]:
            rotated_path = self.apply_rotation(watermarked_image, angle)
            detected, similarity = self.watermark_system.detect_watermark(
                rotated_path, original_size, watermark_size, expected_message
            )
            results[f"rotation_{angle}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"angle": angle},
            }
            os.remove(rotated_path)

        # 2. 翻转测试
        print("测试翻转攻击...")
        for flip_code in [0, 1, -1]:
            flipped_path = self.apply_flip(watermarked_image, flip_code)
            detected, similarity = self.watermark_system.detect_watermark(
                flipped_path, original_size, watermark_size, expected_message
            )
            results[f"flip_{flip_code}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"flip_code": flip_code},
            }
            os.remove(flipped_path)

        # 3. 裁剪测试
        print("测试裁剪攻击...")
        for crop_ratio in [0.9, 0.8, 0.7, 0.6]:
            cropped_path = self.apply_crop(watermarked_image, crop_ratio)
            detected, similarity = self.watermark_system.detect_watermark(
                cropped_path, original_size, watermark_size, expected_message
            )
            results[f"crop_{crop_ratio}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"crop_ratio": crop_ratio},
            }
            os.remove(cropped_path)

        # 4. 噪声测试
        print("测试噪声攻击...")
        for noise_level in [0.05, 0.1, 0.15, 0.2]:
            noisy_path = self.apply_noise(watermarked_image, noise_level)
            detected, similarity = self.watermark_system.detect_watermark(
                noisy_path, original_size, watermark_size, expected_message
            )
            results[f"noise_{noise_level}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"noise_level": noise_level},
            }
            os.remove(noisy_path)

        # 5. 压缩测试
        print("测试压缩攻击...")
        for quality in [90, 80, 70, 60, 50]:
            compressed_path = self.apply_compression(watermarked_image, quality)
            detected, similarity = self.watermark_system.detect_watermark(
                compressed_path, original_size, watermark_size, expected_message
            )
            results[f"compression_{quality}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"quality": quality},
            }
            os.remove(compressed_path)

        # 6. 亮度对比度测试
        print("测试亮度对比度攻击...")
        for brightness, contrast in [(0.8, 1.2), (1.2, 0.8), (0.7, 1.3), (1.3, 0.7)]:
            adjusted_path = self.apply_brightness_contrast(
                watermarked_image, brightness, contrast
            )
            detected, similarity = self.watermark_system.detect_watermark(
                adjusted_path, original_size, watermark_size, expected_message
            )
            results[f"brightness_contrast_{brightness}_{contrast}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"brightness": brightness, "contrast": contrast},
            }
            os.remove(adjusted_path)

        # 7. 模糊测试
        print("测试模糊攻击...")
        for blur_radius in [0.5, 1.0, 1.5, 2.0]:
            blurred_path = self.apply_blur(watermarked_image, blur_radius)
            detected, similarity = self.watermark_system.detect_watermark(
                blurred_path, original_size, watermark_size, expected_message
            )
            results[f"blur_{blur_radius}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"blur_radius": blur_radius},
            }
            os.remove(blurred_path)

        # 8. 缩放测试
        print("测试缩放攻击...")
        for scale_factor in [0.8, 0.9, 1.1, 1.2]:
            scaled_path = self.apply_scaling(watermarked_image, scale_factor)
            detected, similarity = self.watermark_system.detect_watermark(
                scaled_path, original_size, watermark_size, expected_message
            )
            results[f"scale_{scale_factor}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"scale_factor": scale_factor},
            }
            os.remove(scaled_path)

        # 9. 平移测试
        print("测试平移攻击...")
        for dx, dy in [(10, 10), (20, 20), (30, 30), (50, 50), (-10, -10)]:
            translated_path = self.apply_translation(watermarked_image, dx, dy)
            detected, similarity = self.watermark_system.detect_watermark(
                translated_path, original_size, watermark_size, expected_message
            )
            results[f"translation_{dx}_{dy}"] = {
                "detected": detected,
                "similarity": similarity,
                "attack_params": {"dx": dx, "dy": dy},
            }
            os.remove(translated_path)

        self.test_results = results
        return results

    def generate_report(self, output_file: str = "robustness_report.txt"):
        """
        生成测试报告

        Args:
            output_file: 输出文件路径
        """
        if not self.test_results:
            print("没有测试结果可生成报告")
            return

        with open(output_file, "w", encoding="utf-8") as f:
            f.write("数字水印鲁棒性测试报告\n")
            f.write("=" * 50 + "\n\n")

            # 统计信息
            total_tests = len(self.test_results)
            detected_count = sum(
                1 for result in self.test_results.values() if result["detected"]
            )
            avg_similarity = np.mean(
                [result["similarity"] for result in self.test_results.values()]
            )

            f.write(f"总测试数: {total_tests}\n")
            f.write(f"检测成功数: {detected_count}\n")
            f.write(f"检测成功率: {detected_count/total_tests*100:.2f}%\n")
            f.write(f"平均相似度: {avg_similarity:.4f}\n\n")

            # 详细结果
            f.write("详细测试结果:\n")
            f.write("-" * 30 + "\n")

            for test_name, result in self.test_results.items():
                status = "✓" if result["detected"] else "✗"
                f.write(
                    f"{test_name:25} | {status} | 相似度: {result['similarity']:.4f}\n"
                )

            f.write("\n攻击类型分析:\n")
            f.write("-" * 20 + "\n")

            # 按攻击类型分组
            attack_types = {}
            for test_name, result in self.test_results.items():
                attack_type = test_name.split("_")[0]
                if attack_type not in attack_types:
                    attack_types[attack_type] = []
                attack_types[attack_type].append(result)

            for attack_type, results in attack_types.items():
                success_rate = (
                    sum(1 for r in results if r["detected"]) / len(results) * 100
                )
                avg_sim = np.mean([r["similarity"] for r in results])
                f.write(
                    f"{attack_type:15} | 成功率: {success_rate:6.2f}% | 平均相似度: {avg_sim:.4f}\n"
                )

        print(f"测试报告已保存到: {output_file}")

    def plot_results(self, output_file: str = "robustness_plot.png"):
        """
        绘制测试结果图表

        Args:
            output_file: 输出文件路径
        """
        if not self.test_results:
            print("没有测试结果可绘制图表")
            return

        # 准备数据
        test_names = list(self.test_results.keys())
        similarities = [self.test_results[name]["similarity"] for name in test_names]
        detected = [self.test_results[name]["detected"] for name in test_names]

        # 创建图表
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))

        # 相似度柱状图
        bars = ax1.bar(
            range(len(test_names)),
            similarities,
            color=["green" if d else "red" for d in detected],
        )
        # 使用bars变量避免未使用警告
        _ = bars
        ax1.set_title("水印鲁棒性测试结果 - 相似度")
        ax1.set_ylabel("相似度")
        ax1.set_ylim(0, 1)
        ax1.axhline(y=0.7, color="orange", linestyle="--", label="检测阈值")
        ax1.legend()

        # 旋转x轴标签
        ax1.set_xticks(range(len(test_names)))
        ax1.set_xticklabels(test_names, rotation=45, ha="right")

        # 检测成功率饼图
        detected_count = sum(detected)
        undetected_count = len(detected) - detected_count

        ax2.pie(
            [detected_count, undetected_count],
            labels=["检测成功", "检测失败"],
            colors=["lightgreen", "lightcoral"],
            autopct="%1.1f%%",
        )
        ax2.set_title("水印检测成功率")

        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches="tight")
        plt.close()

        print(f"测试结果图表已保存到: {output_file}")
