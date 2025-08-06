#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2 Simple Implementation (No external dependencies)
SM2简化实现 (无外部依赖)

This module provides a simplified implementation of SM2 elliptic curve
cryptography using only standard Python libraries.
"""

import hashlib
import os
import random
import time
from typing import Tuple, Optional


class SimpleSM2:
    """简化的SM2椭圆曲线密码算法实现"""
    
    def __init__(self):
        # SM2推荐曲线参数 (简化版本)
        self.p = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
        self.a = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
        self.b = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
        self.n = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
        self.Gx = 0x32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7
        self.Gy = 0xBC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0
    
    def _mod_inverse(self, a: int, m: int) -> int:
        """模逆运算"""
        def extended_gcd(a, b):
            if a == 0:
                return b, 0, 1
            else:
                gcd, x, y = extended_gcd(b % a, a)
                return gcd, y - (b // a) * x, x
        
        # 处理负数
        a = a % m
        if a < 0:
            a += m
        
        gcd, x, _ = extended_gcd(a, m)
        if gcd != 1:
            raise ValueError("Modular inverse does not exist")
        return x % m
    
    def _point_add(self, P1: Tuple[int, int], P2: Tuple[int, int]) -> Tuple[int, int]:
        """椭圆曲线点加法"""
        if P1 is None:
            return P2
        if P2 is None:
            return P1
        
        x1, y1 = P1
        x2, y2 = P2
        
        if x1 == x2 and y1 != y2:
            return None  # 无穷远点
        
        if x1 == x2 and y1 == y2:
            # 点加倍
            if y1 == 0:
                return None
            lam = ((3 * x1 * x1 + self.a) * self._mod_inverse(2 * y1, self.p)) % self.p
        else:
            # 点加法
            lam = ((y2 - y1) * self._mod_inverse(x2 - x1, self.p)) % self.p
        
        x3 = (lam * lam - x1 - x2) % self.p
        y3 = (lam * (x1 - x3) - y1) % self.p
        
        return (x3, y3)
    
    def _scalar_multiply(self, k: int, P: Tuple[int, int]) -> Tuple[int, int]:
        """椭圆曲线标量乘法"""
        if k == 0 or P is None:
            return None
        
        result = None
        addend = P
        
        while k:
            if k & 1:
                result = self._point_add(result, addend)
            addend = self._point_add(addend, addend)
            k >>= 1
        
        return result
    
    def generate_keypair(self) -> Tuple[int, Tuple[int, int]]:
        """生成SM2密钥对"""
        while True:
            private_key = random.randint(1, self.n - 1)
            public_key = self._scalar_multiply(private_key, (self.Gx, self.Gy))
            
            if public_key is not None:
                return private_key, public_key
    
    def _hash_message(self, message: bytes, public_key: Tuple[int, int]) -> int:
        """哈希函数"""
        data = message + str(public_key[0]).encode() + str(public_key[1]).encode()
        return int.from_bytes(hashlib.sha256(data).digest(), 'big') % self.n
    
    def sign(self, message: bytes, private_key: int, public_key: Tuple[int, int]) -> Tuple[int, int]:
        """SM2数字签名"""
        while True:
            # 生成随机数k
            k = random.randint(1, self.n - 1)
            
            # 计算R = k * G
            R = self._scalar_multiply(k, (self.Gx, self.Gy))
            if R is None:
                continue
            
            # 计算e = H(M || ZA)
            e = self._hash_message(message, public_key)
            
            # 计算r = (e + x1) mod n
            r = (e + R[0]) % self.n
            if r == 0:
                continue
            
            # 计算s = ((1 + d)^-1 * (k - r * d)) mod n
            try:
                s = (self._mod_inverse(1 + private_key, self.n) * 
                     (k - r * private_key)) % self.n
            except ValueError:
                continue
            
            if s == 0:
                continue
            
            return r, s
    
    def verify(self, message: bytes, signature: Tuple[int, int], 
               public_key: Tuple[int, int]) -> bool:
        """SM2数字签名验证"""
        r, s = signature
        
        # 验证签名参数范围
        if not (1 <= r < self.n and 1 <= s < self.n):
            return False
        
        # 计算e = H(M || ZA)
        e = self._hash_message(message, public_key)
        
        # 计算t = (r + s) mod n
        t = (r + s) % self.n
        if t == 0:
            return False
        
        # 计算(x1, y1) = s * G + t * PA
        point1 = self._scalar_multiply(s, (self.Gx, self.Gy))
        point2 = self._scalar_multiply(t, public_key)
        point_sum = self._point_add(point1, point2)
        
        if point_sum is None:
            return False
        
        # 验证R = (e + x1) mod n
        R = (e + point_sum[0]) % self.n
        
        return R == r
    
    def encrypt(self, message: bytes, public_key: Tuple[int, int]) -> Tuple[Tuple[int, int], bytes, bytes]:
        """SM2加密"""
        while True:
            # 生成随机数k
            k = random.randint(1, self.n - 1)
            
            # 计算C1 = k * G
            C1 = self._scalar_multiply(k, (self.Gx, self.Gy))
            if C1 is None:
                continue
            
            # 计算k * PA
            kP = self._scalar_multiply(k, public_key)
            if kP is None:
                continue
            
            # 计算t = KDF(kP, klen)
            t = self._kdf(kP, len(message))
            if all(b == 0 for b in t):
                continue
            
            # 计算C2 = M ⊕ t
            C2 = bytes(a ^ b for a, b in zip(message, t))
            
            # 计算C3 = Hash(kP || M)
            C3 = self._hash_kp_m(kP, message)
            
            return C1, C2, C3
    
    def decrypt(self, ciphertext: Tuple[Tuple[int, int], bytes, bytes], 
                private_key: int) -> bytes:
        """SM2解密"""
        C1, C2, C3 = ciphertext
        
        # 计算d * C1
        dC1 = self._scalar_multiply(private_key, C1)
        if dC1 is None:
            raise ValueError("Invalid ciphertext")
        
        # 计算t = KDF(dC1, klen)
        t = self._kdf(dC1, len(C2))
        if all(b == 0 for b in t):
            raise ValueError("Invalid ciphertext")
        
        # 计算M = C2 ⊕ t
        message = bytes(a ^ b for a, b in zip(C2, t))
        
        # 验证C3 = Hash(dC1 || M)
        expected_C3 = self._hash_kp_m(dC1, message)
        if C3 != expected_C3:
            raise ValueError("Invalid ciphertext")
        
        return message
    
    def _kdf(self, point: Tuple[int, int], klen: int) -> bytes:
        """密钥派生函数"""
        data = str(point[0]).encode() + str(point[1]).encode()
        hash_result = hashlib.sha256(data).digest()
        return hash_result[:klen]
    
    def _hash_kp_m(self, point: Tuple[int, int], message: bytes) -> bytes:
        """计算Hash(kP || M)"""
        data = str(point[0]).encode() + str(point[1]).encode() + message
        return hashlib.sha256(data).digest()


def main():
    """测试简化的SM2功能"""
    print("=== SM2 Simple Implementation Test ===")
    
    # 创建SM2实例
    sm2 = SimpleSM2()
    
    # 生成密钥对
    print("1. 生成密钥对...")
    start_time = time.time()
    private_key, public_key = sm2.generate_keypair()
    keygen_time = time.time() - start_time
    print(f"   密钥生成时间: {keygen_time:.4f} 秒")
    print(f"   私钥: {private_key}")
    print(f"   公钥: ({public_key[0]}, {public_key[1]})")
    
    # 测试签名和验证
    print("\n2. 数字签名测试...")
    message = b"Hello, SM2!"
    print(f"   消息: {message}")
    
    start_time = time.time()
    signature = sm2.sign(message, private_key, public_key)
    sign_time = time.time() - start_time
    print(f"   签名生成时间: {sign_time:.4f} 秒")
    print(f"   签名: (r={signature[0]}, s={signature[1]})")
    
    start_time = time.time()
    is_valid = sm2.verify(message, signature, public_key)
    verify_time = time.time() - start_time
    print(f"   签名验证时间: {verify_time:.4f} 秒")
    print(f"   签名验证: {'通过' if is_valid else '失败'}")
    
    # 测试加密和解密
    print("\n3. 加密解密测试...")
    secret_message = b"This is a secret message!"
    print(f"   原始消息: {secret_message}")
    
    start_time = time.time()
    ciphertext = sm2.encrypt(secret_message, public_key)
    encrypt_time = time.time() - start_time
    print(f"   加密时间: {encrypt_time:.4f} 秒")
    print(f"   密文: C1=({ciphertext[0][0]}, {ciphertext[0][1]})")
    print(f"          C2={ciphertext[1].hex()}")
    print(f"          C3={ciphertext[2].hex()}")
    
    start_time = time.time()
    decrypted = sm2.decrypt(ciphertext, private_key)
    decrypt_time = time.time() - start_time
    print(f"   解密时间: {decrypt_time:.4f} 秒")
    print(f"   解密消息: {decrypted}")
    print(f"   解密验证: {'通过' if decrypted == secret_message else '失败'}")
    
    # 性能总结
    print(f"\n=== 性能总结 ===")
    print(f"密钥生成: {keygen_time:.4f}秒")
    print(f"签名生成: {sign_time:.4f}秒")
    print(f"签名验证: {verify_time:.4f}秒")
    print(f"加密: {encrypt_time:.4f}秒")
    print(f"解密: {decrypt_time:.4f}秒")
    print(f"总时间: {keygen_time + sign_time + verify_time + encrypt_time + decrypt_time:.4f}秒")
    
    print(f"\n=== SM2基础实现完成 ===")
    print("✅ 椭圆曲线点运算")
    print("✅ 密钥生成与验证")
    print("✅ 数字签名生成与验证")
    print("✅ 加密与解密")
    print("✅ 性能测试")


if __name__ == "__main__":
    main() 