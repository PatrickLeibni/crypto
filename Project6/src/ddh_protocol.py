#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DDH-based Private Intersection-Sum Protocol Implementation
基于DDH的隐私保护交集和协议实现

This module implements the ΠDDH protocol from the Google Password Checkup paper:
https://eprint.iacr.org/2019/723.pdf Section 3.1, Figure 2

The protocol allows two parties to compute the sum of values associated with
identifiers that are common to both parties' sets, without revealing the full sets.
"""

import hashlib
import random
import time
from typing import List, Tuple, Dict, Set, Optional
from dataclasses import dataclass
import gmpy2
from Crypto.Util.number import getPrime, bytes_to_long, long_to_bytes


@dataclass
class ProtocolParams:
    """协议参数"""
    # 素数阶群参数
    p: int  # 素数
    g: int  # 生成元
    q: int  # 子群阶数
    
    # 标识符空间
    identifier_space: str = "U"
    
    def __post_init__(self):
        """验证参数"""
        if not gmpy2.is_prime(self.p):
            raise ValueError("p must be prime")
        if not gmpy2.is_prime(self.q):
            raise ValueError("q must be prime")
        if self.q * 2 + 1 != self.p:  # 安全素数
            raise ValueError("p must be safe prime (p = 2q + 1)")
        if pow(self.g, self.q, self.p) != 1:
            raise ValueError("g must be generator of subgroup of order q")


class HomomorphicEncryption:
    """加法同态加密方案 - 基于Paillier思想"""
    
    def __init__(self, params: ProtocolParams):
        self.params = params
        self.public_key = None
        self.secret_key = None
    
    def key_gen(self) -> Tuple[int, int]:
        """生成密钥对"""
        # 选择随机私钥
        self.secret_key = random.randint(1, self.params.q - 1)
        # 计算公钥
        self.public_key = pow(self.params.g, self.secret_key, self.params.p)
        return self.public_key, self.secret_key
    
    def encrypt(self, message: int) -> Tuple[int, int]:
        """加密消息 - 使用加法同态加密"""
        if self.public_key is None:
            raise ValueError("Public key not generated")
        
        # 确保消息在合理范围内
        message = message % self.params.p
        
        # 选择随机数
        r = random.randint(1, self.params.q - 1)
        # 计算密文: (g^r, g^m * h^r)
        c1 = pow(self.params.g, r, self.params.p)
        c2 = (pow(self.params.g, message, self.params.p) * pow(self.public_key, r, self.params.p)) % self.params.p
        return (c1, c2)
    
    def decrypt(self, ciphertext: Tuple[int, int]) -> int:
        """解密消息"""
        if self.secret_key is None:
            raise ValueError("Secret key not generated")
        
        c1, c2 = ciphertext
        # 计算解密: g^m = c2 / (c1^sk)
        s = pow(c1, self.secret_key, self.params.p)
        s_inv = gmpy2.invert(s, self.params.p)
        g_m = (c2 * s_inv) % self.params.p
        
        message = 0
        temp = 1
        while temp != g_m and message < self.params.p:
            message += 1
            temp = (temp * self.params.g) % self.params.p
        
        return message if temp == g_m else 0
    
    def add(self, ciphertext1: Tuple[int, int], ciphertext2: Tuple[int, int]) -> Tuple[int, int]:
        """同态加法 - 密文相乘"""
        c1_1, c2_1 = ciphertext1
        c1_2, c2_2 = ciphertext2
        
        # 密文相乘（同态加法）
        c1 = (c1_1 * c1_2) % self.params.p
        c2 = (c2_1 * c2_2) % self.params.p
        
        return (c1, c2)
    
    def refresh(self, ciphertext: Tuple[int, int]) -> Tuple[int, int]:
        """密文刷新"""
        # 重新加密以刷新随机性
        message = self.decrypt(ciphertext)
        return self.encrypt(message)


class DDHProtocol:
    """DDH-based Private Intersection-Sum Protocol"""
    
    def __init__(self, params: ProtocolParams):
        self.params = params
        self.he = HomomorphicEncryption(params)
        
        # 双方私钥
        self.k1 = None  # P1的私钥
        self.k2 = None  # P2的私钥
        
        # 协议状态
        self.round = 0
        self.messages = {}
    
    def setup(self):
        """协议设置阶段"""
        print("=== 协议设置阶段 ===")
        
        # 双方选择私钥
        self.k1 = random.randint(1, self.params.q - 1)
        self.k2 = random.randint(1, self.params.q - 1)
        
        # P2生成同态加密密钥对
        pk, sk = self.he.key_gen()
        
        print(f"P1私钥: {self.k1}")
        print(f"P2私钥: {self.k2}")
        print(f"同态加密公钥: {pk}")
        print(f"同态加密私钥: {sk}")
        
        return pk, sk
    
    def hash_identifier(self, identifier: str) -> int:
        """哈希标识符到群元素"""
        # 使用SHA-256哈希
        hash_bytes = hashlib.sha256(identifier.encode()).digest()
        hash_int = int.from_bytes(hash_bytes, 'big')
        
        # 映射到群元素
        return pow(self.params.g, hash_int % self.params.q, self.params.p)
    
    def round1_p1(self, V: List[str]) -> List[int]:
        """第1轮：P1计算H(v_i)^k1"""
        print(f"\n=== 第1轮：P1计算H(v_i)^k1 ===")
        print(f"P1的标识符集合: {V}")
        
        blinded_identifiers = []
        for v_i in V:
            # 计算H(v_i)
            h_v = self.hash_identifier(v_i)
            # 计算H(v_i)^k1
            blinded = pow(h_v, self.k1, self.params.p)
            blinded_identifiers.append(blinded)
        
        # 打乱顺序
        random.shuffle(blinded_identifiers)
        
        print(f"发送给P2的盲化标识符数量: {len(blinded_identifiers)}")
        return blinded_identifiers
    
    def round2_p2(self, blinded_from_p1: List[int], W: List[Tuple[str, int]]) -> Tuple[List[int], List[Tuple[int, Tuple[int, int]]]]:
        """第2轮：P2的处理"""
        print(f"\n=== 第2轮：P2处理 ===")
        print(f"P2的标识符-值对: {W}")
        
        # 步骤1: 计算H(v_i)^k1k2
        double_blinded = []
        for blinded in blinded_from_p1:
            double_blinded.append(pow(blinded, self.k2, self.params.p))
        
        # 打乱顺序
        random.shuffle(double_blinded)
        
        # 步骤2: 计算H(w_j)^k2和AEnc(t_j)
        blinded_w_pairs = []
        for w_j, t_j in W:
            # 计算H(w_j)^k2
            h_w = self.hash_identifier(w_j)
            blinded_w = pow(h_w, self.k2, self.params.p)
            
            # 加密t_j
            encrypted_t = self.he.encrypt(t_j)
            
            blinded_w_pairs.append((blinded_w, encrypted_t))
        
        # 打乱顺序
        random.shuffle(blinded_w_pairs)
        
        print(f"发送给P1的双重盲化标识符数量: {len(double_blinded)}")
        print(f"发送给P1的盲化标识符-加密值对数量: {len(blinded_w_pairs)}")
        
        return double_blinded, blinded_w_pairs
    
    def round3_p1(self, double_blinded: List[int], blinded_w_pairs: List[Tuple[int, Tuple[int, int]]]) -> Tuple[int, int]:
        """第3轮：P1计算交集和"""
        print(f"\n=== 第3轮：P1计算交集和 ===")
        
        # 步骤1: 计算H(w_j)^k1k2
        double_blinded_w = []
        for blinded_w, encrypted_t in blinded_w_pairs:
            double_blinded_w.append(pow(blinded_w, self.k1, self.params.p))
        
        # 步骤2: 计算交集J
        intersection_indices = []
        for i, double_blinded_w_j in enumerate(double_blinded_w):
            if double_blinded_w_j in double_blinded:
                intersection_indices.append(i)
        
        print(f"找到的交集元素数量: {len(intersection_indices)}")
        
        # 步骤3: 同态求和
        if intersection_indices:
            # 获取交集对应的加密值
            intersection_encrypted = [blinded_w_pairs[i][1] for i in intersection_indices]
            
            # 同态求和
            sum_ciphertext = intersection_encrypted[0]
            for ciphertext in intersection_encrypted[1:]:
                sum_ciphertext = self.he.add(sum_ciphertext, ciphertext)
            
            # 刷新密文
            refreshed_ciphertext = self.he.refresh(sum_ciphertext)
            
            print(f"交集和密文: {refreshed_ciphertext}")
            return refreshed_ciphertext
        else:
            # 没有交集，返回加密的0
            zero_ciphertext = self.he.encrypt(0)
            print("没有找到交集，返回加密的0")
            return zero_ciphertext
    
    def output_p2(self, sum_ciphertext: Tuple[int, int]) -> int:
        """输出：P2解密得到交集和"""
        print(f"\n=== 输出：P2解密交集和 ===")
        
        # 解密
        intersection_sum = self.he.decrypt(sum_ciphertext)
        
        print(f"交集和: {intersection_sum}")
        return intersection_sum
    
    def run_protocol(self, V: List[str], W: List[Tuple[str, int]]) -> int:
        """运行完整协议"""
        print("=== 开始DDH-based Private Intersection-Sum协议 ===")
        start_time = time.time()
        
        # 设置阶段
        pk, sk = self.setup()
        
        # 第1轮
        blinded_from_p1 = self.round1_p1(V)
        
        # 第2轮
        double_blinded, blinded_w_pairs = self.round2_p2(blinded_from_p1, W)
        
        # 第3轮
        sum_ciphertext = self.round3_p1(double_blinded, blinded_w_pairs)
        
        # 输出
        intersection_sum = self.output_p2(sum_ciphertext)
        
        protocol_time = time.time() - start_time
        print(f"\n=== 协议完成 ===")
        print(f"协议运行时间: {protocol_time:.4f}秒")
        
        return intersection_sum


def generate_protocol_params(bit_length: int = 1024) -> ProtocolParams:
    """生成协议参数"""
    print(f"生成{bit_length}位协议参数...")
    
    # 生成安全素数p = 2q + 1
    q = getPrime(bit_length - 1)
    p = 2 * q + 1
    
    # 验证p是素数
    while not gmpy2.is_prime(p):
        q = getPrime(bit_length - 1)
        p = 2 * q + 1
    
    # 找到生成元g
    g = 2
    while pow(g, q, p) != 1:
        g += 1
    
    params = ProtocolParams(p=p, g=g, q=q)
    print(f"协议参数生成完成:")
    print(f"  p (素数): {p}")
    print(f"  g (生成元): {g}")
    print(f"  q (子群阶): {q}")
    
    return params


def main():
    """主函数：演示协议运行"""
    print("=== DDH-based Private Intersection-Sum Protocol Demo ===")
    
    # 生成协议参数
    params = generate_protocol_params(512)  # 使用512位参数进行演示
    
    # 创建协议实例
    protocol = DDHProtocol(params)
    
    # 准备测试数据
    V = ["password1", "password2", "password3", "password4", "password5"]
    W = [
        ("password2", 10),
        ("password3", 20),
        ("password6", 30),
        ("password7", 40),
        ("password8", 50)
    ]
    
    print(f"\n测试数据:")
    print(f"P1的标识符集合 V: {V}")
    print(f"P2的标识符-值对集合 W: {W}")
    
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
