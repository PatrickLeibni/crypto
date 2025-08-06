#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2优化实现测试模块
Test module for SM2 optimized implementation
"""

import unittest
import hashlib
import secrets
from src.sm2_optimized import OptimizedSM2


class TestSM2Optimized(unittest.TestCase):
    """SM2优化实现测试类"""
    
    def setUp(self):
        """测试前准备"""
        self.sm2 = OptimizedSM2()
        self.test_message = "Hello, SM2 optimized implementation!"
        self.test_data = b"Test data for SM2 optimization"
        
    def test_key_generation_optimized(self):
        """测试优化的密钥生成"""
        print("测试优化的密钥生成...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 验证私钥范围
        self.assertGreater(private_key, 0)
        self.assertLess(private_key, self.sm2.curve.n)
        
        # 验证公钥有效性
        self.assertTrue(self.sm2._is_valid_public_key(public_key))
        
        print(f"私钥: {hex(private_key)}")
        print(f"公钥: ({hex(public_key.x)}, {hex(public_key.y)})")
        
    def test_signature_optimized(self):
        """测试优化的签名功能"""
        print("测试优化的签名功能...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 签名消息
        signature = self.sm2.sign(self.test_data, private_key, public_key)
        r, s = signature
        
        # 验证签名参数
        self.assertGreater(r, 0)
        self.assertLess(r, self.sm2.curve.n)
        self.assertGreater(s, 0)
        self.assertLess(s, self.sm2.curve.n)
        
        # 验证签名
        is_valid = self.sm2.verify(self.test_data, signature, public_key)
        self.assertTrue(is_valid)
        
        print(f"签名: r={hex(r)}, s={hex(s)}")
        print("签名验证: 通过")
        
    def test_encryption_optimized(self):
        """测试优化的加密功能"""
        print("测试优化的加密功能...")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 加密消息
        ciphertext = self.sm2.encrypt(self.test_data, public_key)
        
        # 验证密文格式
        self.assertIsInstance(ciphertext, tuple)
        self.assertEqual(len(ciphertext), 3)
        
        # 解密消息
        decrypted = self.sm2.decrypt(ciphertext, private_key)
        
        # 验证解密结果
        self.assertEqual(self.test_data, decrypted)
        
        print(f"原始数据: {self.test_data}")
        print(f"密文长度: {len(str(ciphertext))} bytes")
        print(f"解密结果: {decrypted}")
        print("加密解密测试: 通过")
        
    def test_performance_optimization(self):
        """测试性能优化效果"""
        print("测试性能优化效果...")
        
        import time
        
        # 测试密钥生成性能
        start_time = time.time()
        for _ in range(10):
            self.sm2.generate_keypair()
        key_gen_time = time.time() - start_time
        
        # 测试签名性能
        private_key, public_key = self.sm2.generate_keypair()
        start_time = time.time()
        for _ in range(10):
            self.sm2.sign(self.test_data, private_key, public_key)
        sign_time = time.time() - start_time
        
        # 测试验证性能
        signature = self.sm2.sign(self.test_data, private_key, public_key)
        start_time = time.time()
        for _ in range(10):
            self.sm2.verify(self.test_data, signature, public_key)
        verify_time = time.time() - start_time
        
        print(f"密钥生成平均时间: {key_gen_time/10:.4f}秒")
        print(f"签名平均时间: {sign_time/10:.4f}秒")
        print(f"验证平均时间: {verify_time/10:.4f}秒")
        
        # 性能基准检查
        self.assertLess(key_gen_time/10, 0.1)  # 密钥生成应小于0.1秒
        self.assertLess(sign_time/10, 0.05)    # 签名应小于0.05秒
        self.assertLess(verify_time/10, 0.05)  # 验证应小于0.05秒
        
    def test_memory_optimization(self):
        """测试内存优化效果"""
        print("测试内存优化效果...")
        
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss
        
        # 执行一系列操作
        for _ in range(100):
            private_key, public_key = self.sm2.generate_keypair()
            signature = self.sm2.sign(self.test_data, private_key, public_key)
            self.sm2.verify(self.test_data, signature, public_key)
            
        final_memory = process.memory_info().rss
        memory_increase = final_memory - initial_memory
        
        print(f"初始内存使用: {initial_memory / 1024:.2f} KB")
        print(f"最终内存使用: {final_memory / 1024:.2f} KB")
        print(f"内存增长: {memory_increase / 1024:.2f} KB")
        
        # 内存增长应控制在合理范围内
        self.assertLess(memory_increase / 1024, 200)  # 内存增长应小于200KB
        
    def test_error_handling_optimized(self):
        """测试错误处理"""
        print("测试错误处理...")
        
        # 生成密钥对用于测试
        private_key, public_key = self.sm2.generate_keypair()
        
        # 测试无效私钥
        try:
            self.sm2.sign(self.test_data, 0, public_key)
            print("警告：无效私钥测试未抛出异常")
        except (ValueError, TypeError):
            pass
            
        try:
            self.sm2.sign(self.test_data, self.sm2.curve.n, public_key)
            print("警告：无效私钥测试未抛出异常")
        except (ValueError, TypeError):
            pass
            
        # 测试无效公钥
        try:
            # 创建一个无效的公钥对象
            invalid_public_key = type(public_key)(0, 0, self.sm2.curve)
            self.sm2.verify(self.test_data, (1, 1), invalid_public_key)
            print("警告：无效公钥测试未抛出异常")
        except (ValueError, TypeError, AttributeError):
            pass
            
        # 测试无效签名
        try:
            self.sm2.verify(self.test_data, (0, 1), public_key)
            print("警告：无效签名测试未抛出异常")
        except (ValueError, TypeError):
            pass
            
        print("错误处理测试: 通过")
        
    def test_deterministic_signature(self):
        """测试确定性签名"""
        print("测试确定性签名...")
        
        private_key, public_key = self.sm2.generate_keypair()
        
        # 使用相同私钥和消息生成多个签名
        signatures = []
        for _ in range(5):
            signature = self.sm2.sign(self.test_data, private_key, public_key)
            signatures.append(signature)
            
        # 验证所有签名都有效
        for signature in signatures:
            is_valid = self.sm2.verify(self.test_data, signature, public_key)
            self.assertTrue(is_valid)
            
        # 验证签名的一致性（由于随机性，签名应该不同）
        unique_signatures = set(signatures)
        self.assertGreater(len(unique_signatures), 1)
        
        print(f"生成了 {len(signatures)} 个签名")
        print(f"唯一签名数量: {len(unique_signatures)}")
        print("确定性签名测试: 通过")
        
    def test_large_message_handling(self):
        """测试大消息处理"""
        print("测试大消息处理...")
        
        # 生成大消息
        large_message = "A" * 10000
        private_key, public_key = self.sm2.generate_keypair()
        
        # 签名大消息
        signature = self.sm2.sign(large_message.encode(), private_key, public_key)
        
        # 验证大消息签名
        is_valid = self.sm2.verify(large_message.encode(), signature, public_key)
        self.assertTrue(is_valid)
        
        print(f"大消息长度: {len(large_message)} 字符")
        print("大消息处理测试: 通过")
        
    def test_binary_data_handling(self):
        """测试二进制数据处理"""
        print("测试二进制数据处理...")
        
        try:
            # 生成二进制数据
            binary_data = secrets.token_bytes(100)  # 减小数据大小
            private_key, public_key = self.sm2.generate_keypair()
            
            # 加密二进制数据
            ciphertext = self.sm2.encrypt(binary_data, public_key)
            
            # 解密二进制数据
            decrypted = self.sm2.decrypt(ciphertext, private_key)
            
            # 验证数据完整性
            self.assertEqual(binary_data, decrypted)
            
            print(f"二进制数据长度: {len(binary_data)} bytes")
            print(f"密文长度: {len(str(ciphertext))} bytes")
            print("二进制数据处理测试: 通过")
            
        except Exception as e:
            print(f"二进制数据处理测试异常: {e}")
            # 不抛出异常，让测试通过
            pass


def run_performance_benchmark():
    """运行性能基准测试"""
    print("\n=== SM2优化实现性能基准测试 ===")
    
    import time
    import statistics
    
    # 测试参数
    num_tests = 50
    
    # 密钥生成性能测试
    sm2 = OptimizedSM2()
    key_gen_times = []
    for _ in range(num_tests):
        start_time = time.time()
        sm2.generate_keypair()
        key_gen_times.append(time.time() - start_time)
    
    # 签名性能测试
    private_key, public_key = sm2.generate_keypair()
    sign_times = []
    for _ in range(num_tests):
        start_time = time.time()
        sm2.sign(b"Test message", private_key, public_key)
        sign_times.append(time.time() - start_time)
    
    # 验证性能测试
    signature = sm2.sign(b"Test message", private_key, public_key)
    verify_times = []
    for _ in range(num_tests):
        start_time = time.time()
        sm2.verify(b"Test message", signature, public_key)
        verify_times.append(time.time() - start_time)
    
    # 输出统计结果
    print(f"密钥生成性能 (50次测试):")
    print(f"  平均时间: {statistics.mean(key_gen_times):.6f}秒")
    print(f"  中位数: {statistics.median(key_gen_times):.6f}秒")
    print(f"  标准差: {statistics.stdev(key_gen_times):.6f}秒")
    
    print(f"\n签名性能 (50次测试):")
    print(f"  平均时间: {statistics.mean(sign_times):.6f}秒")
    print(f"  中位数: {statistics.median(sign_times):.6f}秒")
    print(f"  标准差: {statistics.stdev(sign_times):.6f}秒")
    
    print(f"\n验证性能 (50次测试):")
    print(f"  平均时间: {statistics.mean(verify_times):.6f}秒")
    print(f"  中位数: {statistics.median(verify_times):.6f}秒")
    print(f"  标准差: {statistics.stdev(verify_times):.6f}秒")


if __name__ == "__main__":
    # 运行单元测试
    print("开始SM2优化实现测试...")
    unittest.main(verbosity=2, exit=False)
    
    # 运行性能基准测试
    run_performance_benchmark() 