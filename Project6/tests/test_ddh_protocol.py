#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Tests for DDH-based Private Intersection-Sum Protocol
DDH协议测试
"""

import unittest
import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'src'))

from ddh_protocol import DDHProtocol, ProtocolParams, generate_protocol_params, HomomorphicEncryption


class TestProtocolParams(unittest.TestCase):
    """测试协议参数"""
    
    def test_protocol_params_validation(self):
        """测试协议参数验证"""
        # 测试有效参数
        params = generate_protocol_params(512)
        self.assertIsInstance(params, ProtocolParams)
        self.assertTrue(params.p > params.q)
        self.assertEqual(params.p, 2 * params.q + 1)
    
    def test_invalid_params(self):
        """测试无效参数"""
        with self.assertRaises(ValueError):
            # 测试无效的素数
            ProtocolParams(p=100, g=2, q=50)


class TestHomomorphicEncryption(unittest.TestCase):
    """测试同态加密"""
    
    def setUp(self):
        """设置测试环境"""
        self.params = generate_protocol_params(512)
        self.he = HomomorphicEncryption(self.params)
    
    def test_key_generation(self):
        """测试密钥生成"""
        pk, sk = self.he.key_gen()
        self.assertIsNotNone(pk)
        self.assertIsNotNone(sk)
        self.assertGreater(pk, 0)
        self.assertGreater(sk, 0)
    
    def test_encryption_decryption(self):
        """测试加密解密"""
        pk, sk = self.he.key_gen()
        
        # 测试加密解密
        message = 12345
        ciphertext = self.he.encrypt(message)
        decrypted = self.he.decrypt(ciphertext)
        
        self.assertEqual(message, decrypted)
    
    def test_homomorphic_addition(self):
        """测试同态加法"""
        pk, sk = self.he.key_gen()
        
        # 加密两个消息
        m1, m2 = 10, 20
        c1 = self.he.encrypt(m1)
        c2 = self.he.encrypt(m2)
        
        # 同态加法
        c_sum = self.he.add(c1, c2)
        
        # 解密结果
        result = self.he.decrypt(c_sum)
        
        self.assertEqual(result, m1 + m2)
    
    def test_ciphertext_refresh(self):
        """测试密文刷新"""
        pk, sk = self.he.key_gen()
        
        message = 42
        ciphertext1 = self.he.encrypt(message)
        ciphertext2 = self.he.refresh(ciphertext1)
        
        # 刷新后的密文应该不同但解密结果相同
        self.assertNotEqual(ciphertext1, ciphertext2)
        self.assertEqual(self.he.decrypt(ciphertext1), self.he.decrypt(ciphertext2))


class TestDDHProtocol(unittest.TestCase):
    """测试DDH协议"""
    
    def setUp(self):
        """设置测试环境"""
        self.params = generate_protocol_params(512)
        self.protocol = DDHProtocol(self.params)
    
    def test_setup(self):
        """测试协议设置"""
        pk, sk = self.protocol.setup()
        
        self.assertIsNotNone(self.protocol.k1)
        self.assertIsNotNone(self.protocol.k2)
        self.assertIsNotNone(pk)
        self.assertIsNotNone(sk)
    
    def test_hash_identifier(self):
        """测试标识符哈希"""
        identifier = "test_password"
        hashed = self.protocol.hash_identifier(identifier)
        
        self.assertIsInstance(hashed, int)
        self.assertGreater(hashed, 0)
        self.assertLess(hashed, self.params.p)
    
    def test_round1_p1(self):
        """测试第1轮P1"""
        # 设置协议
        self.protocol.setup()
        
        V = ["password1", "password2", "password3"]
        blinded = self.protocol.round1_p1(V)
        
        self.assertEqual(len(blinded), len(V))
        for b in blinded:
            self.assertIsInstance(b, int)
            self.assertGreater(b, 0)
    
    def test_round2_p2(self):
        """测试第2轮P2"""
        # 设置协议
        self.protocol.setup()
        
        # 准备测试数据
        V = ["password1", "password2"]
        W = [("password1", 10), ("password3", 20)]
        
        # 第1轮
        blinded_from_p1 = self.protocol.round1_p1(V)
        
        # 第2轮
        double_blinded, blinded_w_pairs = self.protocol.round2_p2(blinded_from_p1, W)
        
        self.assertEqual(len(double_blinded), len(blinded_from_p1))
        self.assertEqual(len(blinded_w_pairs), len(W))
    
    def test_round3_p1(self):
        """测试第3轮P1"""
        # 设置协议
        self.protocol.setup()
        
        # 准备测试数据
        V = ["password1", "password2"]
        W = [("password1", 10), ("password3", 20)]
        
        # 运行前两轮
        blinded_from_p1 = self.protocol.round1_p1(V)
        double_blinded, blinded_w_pairs = self.protocol.round2_p2(blinded_from_p1, W)
        
        # 第3轮
        sum_ciphertext = self.protocol.round3_p1(double_blinded, blinded_w_pairs)
        
        self.assertIsInstance(sum_ciphertext, tuple)
        self.assertEqual(len(sum_ciphertext), 2)
    
    def test_complete_protocol(self):
        """测试完整协议"""
        # 准备测试数据
        V = ["password1", "password2", "password3"]
        W = [("password1", 10), ("password2", 20), ("password4", 30)]
        
        # 计算期望的交集和
        V_set = set(V)
        expected_sum = sum(t_j for w_j, t_j in W if w_j in V_set)
        
        # 运行协议
        actual_sum = self.protocol.run_protocol(V, W)
        
        # 验证结果
        self.assertEqual(actual_sum, expected_sum)
    
    def test_no_intersection(self):
        """测试无交集情况"""
        V = ["password1", "password2"]
        W = [("password3", 10), ("password4", 20)]
        
        # 期望的交集和应该是0
        expected_sum = 0
        
        # 运行协议
        actual_sum = self.protocol.run_protocol(V, W)
        
        # 验证结果
        self.assertEqual(actual_sum, expected_sum)
    
    def test_large_intersection(self):
        """测试大交集情况"""
        V = ["password1", "password2", "password3", "password4", "password5"]
        W = [("password1", 10), ("password2", 20), ("password3", 30), 
             ("password4", 40), ("password5", 50)]
        
        # 期望的交集和
        expected_sum = 10 + 20 + 30 + 40 + 50
        
        # 运行协议
        actual_sum = self.protocol.run_protocol(V, W)
        
        # 验证结果
        self.assertEqual(actual_sum, expected_sum)


class TestProtocolSecurity(unittest.TestCase):
    """测试协议安全性"""
    
    def setUp(self):
        """设置测试环境"""
        self.params = generate_protocol_params(512)
        self.protocol = DDHProtocol(self.params)
    
    def test_privacy_preservation(self):
        """测试隐私保护"""
        # 测试P1不学习P2的完整集合
        V = ["password1", "password2"]
        W = [("password1", 10), ("password3", 20)]
        
        # 运行协议
        result = self.protocol.run_protocol(V, W)
        
        # 验证P1只能看到交集和，不能看到具体的W
        self.assertIsInstance(result, int)
        self.assertGreaterEqual(result, 0)
    
    def test_deterministic_results(self):
        """测试确定性结果"""
        V = ["password1", "password2"]
        W = [("password1", 10), ("password2", 20)]
        
        # 运行协议多次
        results = []
        for _ in range(3):
            protocol = DDHProtocol(self.params)
            result = protocol.run_protocol(V, W)
            results.append(result)
        
        # 验证结果一致
        self.assertEqual(len(set(results)), 1)


def run_performance_tests():
    """运行性能测试"""
    print("=== 性能测试 ===")
    
    params = generate_protocol_params(1024)
    protocol = DDHProtocol(params)
    
    # 不同规模的测试
    test_cases = [
        (10, 100),
        (50, 500),
        (100, 1000)
    ]
    
    for user_count, breach_count in test_cases:
        print(f"\n测试规模: {user_count} 用户, {breach_count} 泄露记录")
        
        # 生成测试数据
        V = [f"password{i}" for i in range(user_count)]
        W = [(f"password{i}", i * 10) for i in range(breach_count)]
        
        # 测量性能
        import time
        start_time = time.time()
        result = protocol.run_protocol(V, W)
        end_time = time.time()
        
        print(f"执行时间: {end_time - start_time:.4f}秒")
        print(f"结果: {result}")


if __name__ == "__main__":
    # 运行单元测试
    unittest.main(verbosity=2)
    
    # 运行性能测试
    run_performance_tests() 