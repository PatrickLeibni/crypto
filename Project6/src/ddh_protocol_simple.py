#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DDH-based Private Intersection-Sum Protocol (Simple Version)
基于DDH的隐私保护交集和协议实现 (简化版本)

This module implements the ΠDDH protocol from the Google Password Checkup paper:
https://eprint.iacr.org/2019/723.pdf Section 3.1, Figure 2
"""

import hashlib
import random
import time
from typing import List, Tuple, Dict, Set, Optional
from dataclasses import dataclass


@dataclass
class ProtocolParams:
    """协议参数"""
    # 简化参数
    p: int = 23  # 小素数用于演示
    g: int = 5   # 生成元
    q: int = 11  # 子群阶数
    
    def __post_init__(self):
        """验证参数"""
        # 简化的验证
        if self.p <= 0 or self.g <= 0 or self.q <= 0:
            raise ValueError("参数必须为正数")


class SimpleHomomorphicEncryption:
    """简化的加法同态加密方案"""
    
    def __init__(self, params: ProtocolParams):
        self.params = params
        self.public_key = None
        self.secret_key = None
    
    def key_gen(self) -> Tuple[int, int]:
        """生成密钥对"""
        self.secret_key = random.randint(1, self.params.q - 1)
        self.public_key = pow(self.params.g, self.secret_key, self.params.p)
        return self.public_key, self.secret_key
    
    def encrypt(self, message: int) -> Tuple[int, int]:
        """加密消息"""
        if self.public_key is None:
            raise ValueError("Public key not generated")
        
        r = random.randint(1, self.params.q - 1)
        c1 = pow(self.params.g, r, self.params.p)
        c2 = (message * pow(self.public_key, r, self.params.p)) % self.params.p
        return (c1, c2)
    
    def decrypt(self, ciphertext: Tuple[int, int]) -> int:
        """解密消息"""
        if self.secret_key is None:
            raise ValueError("Secret key not generated")
        
        c1, c2 = ciphertext
        s = pow(c1, self.secret_key, self.params.p)
        # 计算模逆
        s_inv = 1
        for i in range(1, self.params.p):
            if (s * i) % self.params.p == 1:
                s_inv = i
                break
        message = (c2 * s_inv) % self.params.p
        return message
    
    def add(self, ciphertext1: Tuple[int, int], ciphertext2: Tuple[int, int]) -> Tuple[int, int]:
        """同态加法"""
        c1_1, c2_1 = ciphertext1
        c1_2, c2_2 = ciphertext2
        
        c1 = (c1_1 * c1_2) % self.params.p
        c2 = (c2_1 * c2_2) % self.params.p
        
        return (c1, c2)


class SimpleDDHProtocol:
    """简化的DDH协议实现"""
    
    def __init__(self, params: ProtocolParams):
        self.params = params
        self.he = SimpleHomomorphicEncryption(params)
        self.k1 = None
        self.k2 = None
    
    def setup(self):
        """协议设置"""
        print("=== 协议设置阶段 ===")
        
        self.k1 = random.randint(1, self.params.q - 1)
        self.k2 = random.randint(1, self.params.q - 1)
        
        pk, sk = self.he.key_gen()
        
        print(f"P1私钥: {self.k1}")
        print(f"P2私钥: {self.k2}")
        print(f"同态加密公钥: {pk}")
        
        return pk, sk
    
    def hash_identifier(self, identifier: str) -> int:
        """哈希标识符"""
        hash_bytes = hashlib.sha256(identifier.encode()).digest()
        hash_int = int.from_bytes(hash_bytes, 'big')
        return pow(self.params.g, hash_int % self.params.q, self.params.p)
    
    def run_protocol(self, V: List[str], W: List[Tuple[str, int]]) -> int:
        """运行完整协议"""
        print("=== 开始DDH协议 ===")
        start_time = time.time()
        
        # 设置
        pk, sk = self.setup()
        
        # 第1轮：P1计算H(v_i)^k1
        print(f"\n第1轮：P1计算H(v_i)^k1")
        blinded_identifiers = []
        for v_i in V:
            h_v = self.hash_identifier(v_i)
            blinded = pow(h_v, self.k1, self.params.p)
            blinded_identifiers.append(blinded)
        
        # 第2轮：P2处理
        print(f"第2轮：P2处理")
        double_blinded = []
        for blinded in blinded_identifiers:
            double_blinded.append(pow(blinded, self.k2, self.params.p))
        
        blinded_w_pairs = []
        for w_j, t_j in W:
            h_w = self.hash_identifier(w_j)
            blinded_w = pow(h_w, self.k2, self.params.p)
            encrypted_t = self.he.encrypt(t_j)
            blinded_w_pairs.append((blinded_w, encrypted_t))
        
        # 第3轮：P1计算交集和
        print(f"第3轮：P1计算交集和")
        intersection_indices = []
        for i, (blinded_w, encrypted_t) in enumerate(blinded_w_pairs):
            double_blinded_w = pow(blinded_w, self.k1, self.params.p)
            if double_blinded_w in double_blinded:
                intersection_indices.append(i)
        
        print(f"找到的交集元素数量: {len(intersection_indices)}")
        
        # 同态求和
        if intersection_indices:
            intersection_encrypted = [blinded_w_pairs[i][1] for i in intersection_indices]
            sum_ciphertext = intersection_encrypted[0]
            for ciphertext in intersection_encrypted[1:]:
                sum_ciphertext = self.he.add(sum_ciphertext, ciphertext)
        else:
            sum_ciphertext = self.he.encrypt(0)
        
        # 输出：P2解密
        intersection_sum = self.he.decrypt(sum_ciphertext)
        
        protocol_time = time.time() - start_time
        print(f"协议运行时间: {protocol_time:.4f}秒")
        print(f"交集和: {intersection_sum}")
        
        return intersection_sum


def main():
    """主函数"""
    print("=== DDH协议简化演示 ===")
    
    # 使用简化参数
    params = ProtocolParams()
    protocol = SimpleDDHProtocol(params)
    
    # 测试数据
    V = ["password1", "password2", "password3"]
    W = [("password1", 10), ("password2", 20), ("password4", 30)]
    
    print(f"P1的标识符集合: {V}")
    print(f"P2的标识符-值对: {W}")
    
    # 计算期望的交集和
    V_set = set(V)
    expected_sum = sum(t_j for w_j, t_j in W if w_j in V_set)
    print(f"期望的交集和: {expected_sum}")
    
    # 运行协议
    actual_sum = protocol.run_protocol(V, W)
    
    # 验证结果
    print(f"\n=== 结果验证 ===")
    print(f"期望的交集和: {expected_sum}")
    print(f"协议计算的结果: {actual_sum}")
    print(f"结果正确: {'是' if expected_sum == actual_sum else '否'}")
    
    if expected_sum == actual_sum:
        print("✅ 协议运行成功！")
    else:
        print("❌ 协议运行失败！")


if __name__ == "__main__":
    main() 