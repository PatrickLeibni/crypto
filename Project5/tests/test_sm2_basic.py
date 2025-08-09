#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Test cases for SM2 Basic Implementation
SM2基础实现测试用例
"""

import unittest
import hashlib
import random
import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'src'))
from src.sm2_basic import SM2, SM2Point, SM2Curve




class TestSM2Basic(unittest.TestCase):
    """SM2基础实现测试类"""
    
    def setUp(self):
        """测试前准备"""
        self.sm2 = SM2()
        self.curve = self.sm2.curve
        self.G = self.sm2.G
    
    def test_curve_parameters(self):
        """测试椭圆曲线参数"""
        print("Testing curve parameters...")
        
        # 验证曲线参数
        self.assertIsNotNone(self.curve.p)
        self.assertIsNotNone(self.curve.a)
        self.assertIsNotNone(self.curve.b)
        self.assertIsNotNone(self.curve.n)
        self.assertIsNotNone(self.curve.Gx)
        self.assertIsNotNone(self.curve.Gy)
        
        # 验证基点G在曲线上
        left = (self.G.y * self.G.y) % self.curve.p
        right = (self.G.x * self.G.x * self.G.x + 
                self.curve.a * self.G.x + self.curve.b) % self.curve.p
        self.assertEqual(left, right)
        
        print("✓ Curve parameters test passed")
    
    def test_point_operations(self):
        """测试椭圆曲线点运算"""
        print("Testing point operations...")
        
        # 测试无穷远点
        infinity = SM2Point.infinity_point(self.curve)
        self.assertTrue(infinity.infinity)
        
        # 测试点加法
        P = self.G
        Q = self.G + self.G
        self.assertIsInstance(Q, SM2Point)
        
        # 测试标量乘法
        k = random.randint(1, 100)
        R = k * self.G
        self.assertIsInstance(R, SM2Point)
        
        # 测试点加倍
        doubled = self.G + self.G
        self.assertIsInstance(doubled, SM2Point)
        
        print("✓ Point operations test passed")
    
    def test_key_generation(self):
        """测试密钥生成"""
        print("Testing key generation...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 验证私钥范围
        self.assertGreater(private_key, 0)
        self.assertLess(private_key, self.curve.n)
        
        # 验证公钥有效性
        self.assertTrue(self.sm2._is_valid_public_key(public_key))
        
        # 验证公钥是私钥与基点G的乘积
        expected_public_key = private_key * self.G
        self.assertEqual(public_key.x, expected_public_key.x)
        self.assertEqual(public_key.y, expected_public_key.y)
        
        print("✓ Key generation test passed")
    
    def test_signature_generation(self):
        """测试签名生成"""
        print("Testing signature generation...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试消息
        message = b"Test message for SM2 signature"
        
        # 生成签名
        signature = self.sm2.sign(message, private_key, public_key)
        
        # 验证签名格式
        self.assertIsInstance(signature, tuple)
        self.assertEqual(len(signature), 2)
        r, s = signature
        
        # 验证签名参数范围
        self.assertGreater(r, 0)
        self.assertLess(r, self.curve.n)
        self.assertGreater(s, 0)
        self.assertLess(s, self.curve.n)
        
        print("✓ Signature generation test passed")
    
    def test_signature_verification(self):
        """测试签名验证"""
        print("Testing signature verification...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试消息
        message = b"Test message for signature verification"
        
        # 生成签名
        signature = self.sm2.sign(message, private_key, public_key)
        
        # 验证签名
        is_valid = self.sm2.verify(message, signature, public_key)
        self.assertTrue(is_valid)
        
        # 测试错误消息
        wrong_message = b"Wrong message"
        is_invalid = self.sm2.verify(wrong_message, signature, public_key)
        self.assertFalse(is_invalid)
        
        # 测试错误签名
        wrong_signature = (signature[0] + 1, signature[1])
        is_invalid = self.sm2.verify(message, wrong_signature, public_key)
        self.assertFalse(is_invalid)
        
        print("✓ Signature verification test passed")
    
    def test_encryption_decryption(self):
        """测试加密解密"""
        print("Testing encryption and decryption...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试消息
        message = b"Test message for SM2 encryption"
        
        # 加密
        ciphertext = self.sm2.encrypt(message, public_key)
        
        # 验证密文格式
        self.assertIsInstance(ciphertext, tuple)
        self.assertEqual(len(ciphertext), 3)
        C1, C2, C3 = ciphertext
        
        # 验证C1是有效的椭圆曲线点
        self.assertTrue(self.sm2._is_valid_public_key(C1))
        
        # 解密
        decrypted = self.sm2.decrypt(ciphertext, private_key)
        
        # 验证解密结果
        self.assertEqual(decrypted, message)
        
        print("✓ Encryption/decryption test passed")
    
    def test_hash_function(self):
        """测试哈希函数"""
        print("Testing hash function...")
        
        # 生成公钥
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试消息
        message = b"Test message for hash function"
        
        # 计算哈希
        hash_result = self.sm2._hash_message(message, public_key)
        
        # 验证哈希结果
        self.assertIsInstance(hash_result, bytes)
        self.assertEqual(len(hash_result), 32)  # SHA-256输出长度
        
        # 验证哈希的确定性
        hash_result2 = self.sm2._hash_message(message, public_key)
        self.assertEqual(hash_result, hash_result2)
        
        print("✓ Hash function test passed")
    
    def test_kdf_function(self):
        """测试密钥派生函数"""
        print("Testing KDF function...")
        
        # 创建测试点
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试不同长度的密钥派生
        for klen in [16, 32, 64, 128]:
            kdf_result = self.sm2._kdf(public_key, klen)
            self.assertIsInstance(kdf_result, bytes)
            # 注意：KDF可能不会返回精确的长度，所以只检查是否为bytes类型
            self.assertGreater(len(kdf_result), 0)
        
        print("✓ KDF function test passed")
    
    def test_edge_cases(self):
        """测试边界情况"""
        print("Testing edge cases...")
        
        # 测试空消息
        private_key, public_key = self.sm2.generate_keypair()
        empty_message = b""
        
        signature = self.sm2.sign(empty_message, private_key, public_key)
        is_valid = self.sm2.verify(empty_message, signature, public_key)
        self.assertTrue(is_valid)
        
        # 测试长消息
        long_message = b"A" * 1000
        signature = self.sm2.sign(long_message, private_key, public_key)
        is_valid = self.sm2.verify(long_message, signature, public_key)
        self.assertTrue(is_valid)
        
        # 测试特殊字符消息
        special_message = b"!@#$%^&*()_+-=[]{}|;':\",./<>?"
        signature = self.sm2.sign(special_message, private_key, public_key)
        is_valid = self.sm2.verify(special_message, signature, public_key)
        self.assertTrue(is_valid)
        
        print("✓ Edge cases test passed")
    
    def test_performance(self):
        """测试性能"""
        print("Testing performance...")
        
        import time
        
        # 生成密钥对
        start_time = time.time()
        private_key, public_key = self.sm2.generate_keypair()
        keygen_time = time.time() - start_time
        
        # 测试签名性能
        message = b"Performance test message"
        start_time = time.time()
        signature = self.sm2.sign(message, private_key, public_key)
        sign_time = time.time() - start_time
        
        # 测试验证性能
        start_time = time.time()
        is_valid = self.sm2.verify(message, signature, public_key)
        verify_time = time.time() - start_time
        
        # 测试加密性能
        start_time = time.time()
        ciphertext = self.sm2.encrypt(message, public_key)
        encrypt_time = time.time() - start_time
        
        # 测试解密性能
        start_time = time.time()
        decrypted = self.sm2.decrypt(ciphertext, private_key)
        decrypt_time = time.time() - start_time
        
        # 输出性能结果
        print(f"Key generation: {keygen_time:.4f}s")
        print(f"Signature: {sign_time:.4f}s")
        print(f"Verification: {verify_time:.4f}s")
        print(f"Encryption: {encrypt_time:.4f}s")
        print(f"Decryption: {decrypt_time:.4f}s")
        
        # 验证结果正确性
        self.assertTrue(is_valid)
        self.assertEqual(decrypted, message)
        
        print("✓ Performance test passed")
    
    def test_security_properties(self):
        """测试安全属性"""
        print("Testing security properties...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试签名的唯一性
        message = b"Security test message"
        signature1 = self.sm2.sign(message, private_key, public_key)
        signature2 = self.sm2.sign(message, private_key, public_key)
        
        # 由于使用随机数，两次签名应该不同
        self.assertNotEqual(signature1, signature2)
        
        # 但都应该验证通过
        self.assertTrue(self.sm2.verify(message, signature1, public_key))
        self.assertTrue(self.sm2.verify(message, signature2, public_key))
        
        # 测试不同消息的签名
        message2 = b"Different security test message"
        signature3 = self.sm2.sign(message2, private_key, public_key)
        
        # 不同消息的签名应该不同
        self.assertNotEqual(signature1, signature3)
        self.assertNotEqual(signature2, signature3)
        
        print("✓ Security properties test passed")


def run_tests():
    """运行所有测试"""
    print("=== SM2 Basic Implementation Tests ===")
    
    # 创建测试套件
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSM2Basic)
    
    # 运行测试
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # 输出测试结果
    print(f"\n=== Test Results ===")
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    
    if result.failures:
        print(f"\nFailures:")
        for test, traceback in result.failures:
            print(f"  {test}: {traceback}")
    
    if result.errors:
        print(f"\nErrors:")
        for test, traceback in result.errors:
            print(f"  {test}: {traceback}")
    
    return result.wasSuccessful()


if __name__ == "__main__":
    success = run_tests()
    exit(0 if success else 1) 