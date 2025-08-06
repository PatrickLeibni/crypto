import cv2
import numpy as np
import hashlib
import random
from typing import Tuple


class DigitalWatermark:
    """
    数字水印系统 - 基于DCT变换的图片水印嵌入和提取
    """

    def __init__(self, alpha: float = 0.1):
        """
        初始化水印系统

        Args:
            alpha: 水印强度参数 (0.01-0.5)
        """
        self.alpha = alpha
        self.seed = None

    def _set_seed(self, seed: int):
        """设置随机种子以确保可重现性"""
        self.seed = seed
        random.seed(seed)
        np.random.seed(seed)

    def _generate_watermark(self, size: int, message: str = None) -> np.ndarray:
        """
        生成水印序列

        Args:
            size: 水印大小
            message: 自定义消息，如果为None则生成随机水印

        Returns:
            水印序列 (1D numpy array)
        """
        if message:
            # 使用消息生成确定性水印
            hash_obj = hashlib.md5(message.encode())
            hash_bytes = hash_obj.digest()
            # 将每个字节转换为8位二进制
            watermark = []
            for b in hash_bytes:
                watermark.extend([int(bit) for bit in format(b, "08b")])
            watermark = np.array(watermark, dtype=np.uint8)
            # 扩展到指定大小
            watermark = np.tile(watermark, (size // len(watermark)) + 1)[:size]
        else:
            # 生成随机水印
            watermark = np.random.randint(0, 2, size)

        return watermark

    def embed_watermark(
        self,
        image_path: str,
        message: str = None,
        output_path: str = None,
        seed: int = 42,
    ) -> Tuple[np.ndarray, np.ndarray]:
        """
        嵌入水印到图片中

        Args:
            image_path: 输入图片路径
            message: 水印消息
            output_path: 输出图片路径
            seed: 随机种子

        Returns:
            (原始图片, 含水印图片)
        """
        self._set_seed(seed)

        # 读取图片
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        # 转换为灰度图
        if len(img.shape) == 3:
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        else:
            gray = img.copy()

        # 确保图片尺寸是8的倍数
        h, w = gray.shape
        h = (h // 8) * 8
        w = (w // 8) * 8
        gray = gray[:h, :w]

        # 生成水印
        watermark_size = (h // 8) * (w // 8)
        watermark = self._generate_watermark(watermark_size, message)

        # 嵌入水印
        watermarked_img = self._embed_dct(gray, watermark)

        # 保存结果
        if output_path:
            cv2.imwrite(output_path, watermarked_img)

        return gray, watermarked_img

    def _embed_dct(self, image: np.ndarray, watermark: np.ndarray) -> np.ndarray:
        """
        使用DCT变换嵌入水印

        Args:
            image: 输入图片
            watermark: 水印序列

        Returns:
            含水印的图片
        """
        h, w = image.shape
        watermarked = image.copy().astype(np.float64)

        watermark_idx = 0
        for i in range(0, h, 8):
            for j in range(0, w, 8):
                if watermark_idx >= len(watermark):
                    break

                # 提取8x8块
                block = image[i : i + 8, j : j + 8]

                # 确保块是8x8的
                if block.shape != (8, 8):
                    # 如果块不是8x8，跳过
                    continue

                # 应用DCT变换
                dct_block = cv2.dct(block.astype(np.float64))

                # 在DCT系数中嵌入水印 (使用中频系数)
                if watermark[watermark_idx] == 1:
                    dct_block[4, 4] += self.alpha * abs(dct_block[4, 4])
                else:
                    dct_block[4, 4] -= self.alpha * abs(dct_block[4, 4])

                # 应用逆DCT变换
                idct_block = cv2.idct(dct_block)

                # 更新图片
                watermarked[i : i + 8, j : j + 8] = idct_block
                watermark_idx += 1

        return np.clip(watermarked, 0, 255).astype(np.uint8)

    def extract_watermark(
        self,
        image_path: str,
        original_size: Tuple[int, int],
        watermark_size: int,
        seed: int = 42,
    ) -> np.ndarray:
        """
        从图片中提取水印

        Args:
            image_path: 含水印图片路径
            original_size: 原始图片尺寸 (height, width)
            watermark_size: 水印大小
            seed: 随机种子

        Returns:
            提取的水印序列
        """
        self._set_seed(seed)

        # 读取图片
        img = cv2.imread(image_path)
        if img is None:
            raise ValueError(f"无法读取图片: {image_path}")

        # 转换为灰度图
        if len(img.shape) == 3:
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        else:
            gray = img.copy()

        # 确保尺寸匹配
        h, w = gray.shape
        h = min(h, original_size[0])
        w = min(w, original_size[1])
        gray = gray[:h, :w]

        # 提取水印
        extracted_watermark = self._extract_dct(gray, watermark_size)

        return extracted_watermark

    def _extract_dct(self, image: np.ndarray, watermark_size: int) -> np.ndarray:
        """
        使用DCT变换提取水印

        Args:
            image: 含水印图片
            watermark_size: 水印大小

        Returns:
            提取的水印序列
        """
        h, w = image.shape
        extracted_watermark = []

        watermark_idx = 0
        for i in range(0, h, 8):
            for j in range(0, w, 8):
                if watermark_idx >= watermark_size:
                    break

                # 提取8x8块
                block = image[i : i + 8, j : j + 8]

                # 确保块是8x8的
                if block.shape != (8, 8):
                    # 如果块不是8x8，跳过
                    continue

                # 应用DCT变换
                dct_block = cv2.dct(block.astype(np.float64))

                # 从DCT系数中提取水印
                if dct_block[4, 4] > 0:
                    extracted_watermark.append(1)
                else:
                    extracted_watermark.append(0)

                watermark_idx += 1

        return np.array(extracted_watermark)

    def calculate_similarity(
        self, watermark1: np.ndarray, watermark2: np.ndarray
    ) -> float:
        """
        计算两个水印的相似度

        Args:
            watermark1: 第一个水印
            watermark2: 第二个水印

        Returns:
            相似度 (0-1)
        """
        if len(watermark1) != len(watermark2):
            min_len = min(len(watermark1), len(watermark2))
            watermark1 = watermark1[:min_len]
            watermark2 = watermark2[:min_len]

        # 计算汉明距离
        hamming_distance = np.sum(watermark1 != watermark2)
        similarity = 1 - (hamming_distance / len(watermark1))

        return similarity

    def detect_watermark(
        self,
        image_path: str,
        original_size: Tuple[int, int],
        watermark_size: int,
        expected_message: str = None,
        threshold: float = 0.6,
        seed: int = 42,
    ) -> Tuple[bool, float]:
        """
        检测图片中是否存在水印

        Args:
            image_path: 待检测图片路径
            original_size: 原始图片尺寸
            watermark_size: 水印大小
            expected_message: 期望的水印消息
            threshold: 检测阈值
            seed: 随机种子

        Returns:
            (是否检测到水印, 相似度)
        """
        # 提取水印
        extracted_watermark = self.extract_watermark(
            image_path, original_size, watermark_size, seed
        )

        if expected_message:
            # 生成期望的水印
            expected_watermark = self._generate_watermark(
                watermark_size, expected_message
            )
            similarity = self.calculate_similarity(
                extracted_watermark, expected_watermark
            )
        else:
            # 如果没有期望消息，检查水印是否全为0或全为1
            ones_ratio = np.sum(extracted_watermark) / len(extracted_watermark)
            similarity = max(ones_ratio, 1 - ones_ratio)

        detected = similarity >= threshold
        return detected, similarity
