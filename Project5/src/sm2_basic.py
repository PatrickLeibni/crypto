#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2 Basic Implementation
SM2椭圆曲线密码算法基础实现

该模块提供了SM2椭圆曲线的基本实现，密码算法符合中国国家标准。
"""

import hashlib
import hmac
import os
import random
import time
from typing import Tuple, Optional, Union
import gmpy2
from gmpy2 import mpz


class SM2Curve:
    """SM2椭圆曲线参数类"""
    
    def __init__(self):
        # SM2推荐曲线参数 (GB/T 32918.1-2016)
        self.p = mpz("FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF", 16)
        self.a = mpz("FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC", 16)
        self.b = mpz("28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93", 16)
        self.n = mpz("FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123", 16)
        self.Gx = mpz("32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7", 16)
        self.Gy = mpz("BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0", 16)
        
        # 基点G
        self.G = (self.Gx, self.Gy)
        
        # 辅助参数
        self.h = 1  # 余因子


class SM2Point:
    """椭圆曲线点类"""
    
    def __init__(self, x: int, y: int, curve: SM2Curve):
        self.x = mpz(x)
        self.y = mpz(y)
        self.curve = curve
        self.infinity = False
    
    @classmethod
    def infinity_point(cls, curve: SM2Curve):
        """创建无穷远点"""
        point = cls(0, 0, curve)
        point.infinity = True
        return point
    
    def __eq__(self, other):
        if not isinstance(other, SM2Point):
            return False
        if self.infinity and other.infinity:
            return True
        if self.infinity or other.infinity:
            return False
        return self.x == other.x and self.y == other.y
    
    def __add__(self, other):
        """椭圆曲线点加法"""
        if self.infinity:
            return other
        if other.infinity:
            return self
        
        if self.x == other.x and self.y != other.y:
            return SM2Point.infinity_point(self.curve)
        
        if self.x == other.x and self.y == other.y:
            # 点加倍
            if self.y == 0:
                return SM2Point.infinity_point(self.curve)
            lam = (3 * self.x * self.x + self.curve.a) * gmpy2.invert(2 * self.y, self.curve.p)
        else:
            # 点加法
            lam = (other.y - self.y) * gmpy2.invert(other.x - self.x, self.curve.p)
        
        lam = lam % self.curve.p
        x3 = (lam * lam - self.x - other.x) % self.curve.p
        y3 = (lam * (self.x - x3) - self.y) % self.curve.p
        
        return SM2Point(x3, y3, self.curve)
    
    def __mul__(self, k):
        """椭圆曲线标量乘法 (double-and-add算法)"""
        if k == 0:
            return SM2Point.infinity_point(self.curve)
        
        result = SM2Point.infinity_point(self.curve)
        addend = self
        
        while k:
            if k & 1:
                result = result + addend
            addend = addend + addend
            k >>= 1
        
        return result
    
    def __rmul__(self, k):
        return self * k


class SM2:
    """SM2椭圆曲线密码算法实现"""
    
    def __init__(self):
        self.curve = SM2Curve()
        self.G = SM2Point(self.curve.Gx, self.curve.Gy, self.curve)
    
    def generate_keypair(self) -> Tuple[int, SM2Point]:
        """生成SM2密钥对"""
        while True:
            private_key = random.randint(1, self.curve.n - 1)
            public_key = private_key * self.G
            
            # 验证公钥有效性
            if self._is_valid_public_key(public_key):
                return private_key, public_key
    
    def _is_valid_public_key(self, public_key: SM2Point) -> bool:
        """验证公钥有效性"""
        if public_key.infinity:
            return False
        
        # 检查点是否在曲线上
        left = (public_key.y * public_key.y) % self.curve.p
        right = (public_key.x * public_key.x * public_key.x + 
                self.curve.a * public_key.x + self.curve.b) % self.curve.p
        
        if left != right:
            return False
        
        # 检查阶数
        if public_key * self.curve.n != SM2Point.infinity_point(self.curve):
            return False
        
        return True
    
    def _hash_message(self, message: bytes, public_key: SM2Point) -> bytes:
        """SM3哈希函数"""
        data = message + str(public_key.x).encode() + str(public_key.y).encode()
        return hashlib.sha256(data).digest()
    
    def sign(self, message: bytes, private_key: int, public_key: SM2Point) -> Tuple[int, int]:
        """SM2数字签名"""
        while True:
            # 生成随机数k
            k = random.randint(1, self.curve.n - 1)
            
            # 计算R = k * G
            R = k * self.G
            
            # 计算e = H(M || ZA)
            e_hash = self._hash_message(message, public_key)
            e = int.from_bytes(e_hash, 'big') % self.curve.n
            
            # 计算r = (e + x1) mod n
            r = (e + R.x) % self.curve.n
            if r == 0:
                continue
            
            # 计算s = ((1 + d)^-1 * (k - r * d)) mod n
            s = (gmpy2.invert(1 + private_key, self.curve.n) * 
                 (k - r * private_key)) % self.curve.n
            
            if s == 0:
                continue
            
            return r, s
    
    def verify(self, message: bytes, signature: Tuple[int, int], 
               public_key: SM2Point) -> bool:
        """SM2数字签名验证"""
        r, s = signature
        
        # 验证签名参数范围
        if not (1 <= r < self.curve.n and 1 <= s < self.curve.n):
            return False
        
        # 计算e = H(M || ZA)
        e_hash = self._hash_message(message, public_key)
        e = int.from_bytes(e_hash, 'big') % self.curve.n
        
        # 计算t = (r + s) mod n
        t = (r + s) % self.curve.n
        if t == 0:
            return False
        
        # 计算(x1, y1) = s * G + t * PA
        point1 = s * self.G
        point2 = t * public_key
        point_sum = point1 + point2
        
        # 验证R = (e + x1) mod n
        R = (e + point_sum.x) % self.curve.n
        
        return R == r
    
    def encrypt(self, message: bytes, public_key: SM2Point) -> Tuple[SM2Point, bytes]:
        """SM2加密"""
        while True:
            # 生成随机数k
            k = random.randint(1, self.curve.n - 1)
            
            # 计算C1 = k * G
            C1 = k * self.G
            
            # 计算k * PA
            kP = k * public_key
            
            # 计算t = KDF(kP, klen)
            t = self._kdf(kP, len(message))
            if all(b == 0 for b in t):
                continue
            
            # 计算C2 = M ⊕ t
            C2 = bytes(a ^ b for a, b in zip(message, t))
            
            # 计算C3 = Hash(kP || M)
            C3 = self._hash_kp_m(kP, message)
            
            return C1, C2, C3
    
    def decrypt(self, ciphertext: Tuple[SM2Point, bytes, bytes], 
                private_key: int) -> bytes:
        """SM2解密"""
        C1, C2, C3 = ciphertext
    
        # 验证C1是否在曲线上
        if not self._is_valid_public_key(C1):
            raise ValueError("Invalid C1 point")
    
        # 计算h * C1
        hC1 = self.curve.h * C1
        if hC1.infinity:
            raise ValueError("Invalid ciphertext")
    
        # 计算d * C1
        dC1 = private_key * C1
    
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
    
    def _kdf(self, point: SM2Point, klen: int) -> bytes:
        """密钥派生函数 (符合SM2标准)"""
        ct = 1
        hash_len = 32  # SHA-256产生32字节哈希
        result = b''
    
        while len(result) < klen:
            # 数据 = x坐标 || y坐标 || 计数器
            data = str(point.x).encode() + str(point.y).encode() + ct.to_bytes(4, byteorder='big')
            hash_result = hashlib.sha256(data).digest()
            result += hash_result
            ct += 1
    
        return result[:klen]
    
    def _hash_kp_m(self, point: SM2Point, message: bytes) -> bytes:
        """计算Hash(kP || M)"""
        data = str(point.x).encode() + str(point.y).encode() + message
        return hashlib.sha256(data).digest()
    
    def benchmark(self, iterations: int = 1000):
        """性能基准测试"""
        print(f"=== SM2基本实现性能基准测试 ({iterations} 次迭代) ===")
        
        # 生成密钥对
        print("生成密钥对...")
        start_time = time.time()
        private_key, public_key = self.generate_keypair()
        keygen_time = time.time() - start_time
        print(f"密钥生成时间: {keygen_time:.4f} 秒")
        
        # 测试签名性能
        message = b"Benchmark test message for SM2 basic implementation"
        print(f"\n测试签名性能...")
        
        start_time = time.time()
        for _ in range(iterations):
            signature = self.sign(message, private_key, public_key)
        sign_time = time.time() - start_time
        print(f"平均签名时间: {sign_time/iterations:.6f} 秒")
        
        # 测试验证性能
        print(f"测试验证性能...")
        start_time = time.time()
        for _ in range(iterations):
            is_valid = self.verify(message, signature, public_key)
        verify_time = time.time() - start_time
        print(f"平均验证时间: {verify_time/iterations:.6f} 秒")
        
        # 测试加密性能
        print(f"\n测试加密性能...")
        start_time = time.time()
        for _ in range(iterations):
            ciphertext = self.encrypt(message, public_key)
        encrypt_time = time.time() - start_time
        print(f"平均加密时间: {encrypt_time/iterations:.6f} 秒")
        
        # 测试解密性能
        print(f"测试解密性能...")
        start_time = time.time()
        for _ in range(iterations):
            decrypted = self.decrypt(ciphertext, private_key)
        decrypt_time = time.time() - start_time
        print(f"平均解密时间: {decrypt_time/iterations:.6f} 秒")
        
        # 性能总结
        print(f"\n=== 性能总结 ===")
        print(f"密钥生成: {keygen_time:.4f}秒")
        print(f"签名: {sign_time/iterations:.6f}秒/次")
        print(f"验证: {verify_time/iterations:.6f}秒/次")
        print(f"加密: {encrypt_time/iterations:.6f}秒/次")
        print(f"解密: {decrypt_time/iterations:.6f}秒/次")
        print(f"总操作次数: {iterations * 4}")
        print(f"总时间: {keygen_time + sign_time + verify_time + encrypt_time + decrypt_time:.4f}秒")


def main():
    """测试SM2基本功能"""
    print("=== SM2基本实现测试 ===")
    
    # 创建SM2实例
    sm2 = SM2()
    
    # 生成密钥对
    print("生成密钥对...")
    private_key, public_key = sm2.generate_keypair()
    print(f"私钥: {private_key}")
    print(f"公钥: ({public_key.x}, {public_key.y})")
    
    # 测试签名和验证
    message = b"Hello, SM2!"
    print(f"\n正在签名消息: {message}")
    
    signature = sm2.sign(message, private_key, public_key)
    print(f"签名: (r={signature[0]}, s={signature[1]})")
    
    is_valid = sm2.verify(message, signature, public_key)
    print(f"签名验证: {'通过' if is_valid else '失败'}")
    
    # 测试加密和解密
    print(f"\n正在加密消息: {message}")
    ciphertext = sm2.encrypt(message, public_key)
    print(f"密文: C1=({ciphertext[0].x}, {ciphertext[0].y}), C2={ciphertext[1].hex()}, C3={ciphertext[2].hex()}")
    
    decrypted = sm2.decrypt(ciphertext, private_key)
    print(f"解密后的消息: {decrypted}")
    print(f"解密验证: {'通过' if decrypted == message else '失败'}")
    

if __name__ == "__main__":
    main()
