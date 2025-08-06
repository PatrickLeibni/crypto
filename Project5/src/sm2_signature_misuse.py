#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2 Signature Algorithm Misuse POC
SM2签名算法误用攻击验证

This module implements various signature algorithm misuse attacks on SM2
based on the analysis in 20250713-wen-sm2-public.pdf
"""

import hashlib
import os
import random
import time
from typing import Tuple, List, Dict, Optional
import gmpy2
from gmpy2 import mpz
from sm2_basic import SM2, SM2Point, SM2Curve


class SM2SignatureMisuse:
    """SM2签名算法误用攻击类"""
    
    def __init__(self):
        self.sm2 = SM2()
        self.curve = self.sm2.curve
        self.G = self.sm2.G
    
    def replay_attack_demo(self):
        """重放攻击演示"""
        print("=== SM2 Replay Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = b"Original message for replay attack"
        
        # 生成原始签名
        original_signature = self.sm2.sign(message, private_key, public_key)
        print(f"Original signature: (r={original_signature[0]}, s={original_signature[1]})")
        
        # 验证原始签名
        is_valid = self.sm2.verify(message, original_signature, public_key)
        print(f"Original signature verification: {'PASS' if is_valid else 'FAIL'}")
        
        # 重放攻击：使用相同的签名验证不同的消息
        different_message = b"Different message for replay attack"
        is_replay_valid = self.sm2.verify(different_message, original_signature, public_key)
        print(f"Replay attack verification: {'SUCCESS' if is_replay_valid else 'FAILED'}")
        
        # 分析重放攻击的原理
        print(f"\n=== Replay Attack Analysis ===")
        print("重放攻击原理：")
        print("1. 攻击者截获有效的SM2签名")
        print("2. 将签名重放到不同的消息上")
        print("3. 如果签名验证通过，说明存在重放漏洞")
        print("4. 防护措施：在签名中包含时间戳或随机数")
        
        return is_replay_valid
    
    def nonce_reuse_attack_demo(self):
        """随机数重用攻击演示"""
        print("\n=== SM2 Nonce Reuse Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 模拟随机数重用
        reused_k = random.randint(1, self.curve.n - 1)
        
        # 使用相同随机数k生成两个不同消息的签名
        message1 = b"First message with reused nonce"
        message2 = b"Second message with reused nonce"
        
        # 手动生成签名（模拟随机数重用）
        signature1 = self._sign_with_fixed_k(message1, private_key, public_key, reused_k)
        signature2 = self._sign_with_fixed_k(message2, private_key, public_key, reused_k)
        
        print(f"Message 1: {message1}")
        print(f"Signature 1: (r={signature1[0]}, s={signature1[1]})")
        print(f"Message 2: {message2}")
        print(f"Signature 2: (r={signature2[0]}, s={signature2[1]})")
        
        # 验证签名
        is_valid1 = self.sm2.verify(message1, signature1, public_key)
        is_valid2 = self.sm2.verify(message2, signature2, public_key)
        print(f"Signature 1 verification: {'PASS' if is_valid1 else 'FAIL'}")
        print(f"Signature 2 verification: {'PASS' if is_valid2 else 'FAIL'}")
        
        # 尝试从重用随机数中恢复私钥
        recovered_private_key = self._recover_private_key_from_nonce_reuse(
            message1, signature1, message2, signature2, public_key
        )
        
        if recovered_private_key:
            print(f"Recovered private key: {recovered_private_key}")
            print(f"Original private key: {private_key}")
            print(f"Recovery successful: {'YES' if recovered_private_key == private_key else 'NO'}")
        else:
            print("Private key recovery failed")
        
        # 分析随机数重用攻击
        print(f"\n=== Nonce Reuse Attack Analysis ===")
        print("随机数重用攻击原理：")
        print("1. 如果两个签名使用相同的随机数k")
        print("2. 可以建立线性方程组求解私钥")
        print("3. 防护措施：确保每次签名使用不同的随机数")
        
        return recovered_private_key == private_key
    
    def _sign_with_fixed_k(self, message: bytes, private_key: int, 
                          public_key: SM2Point, k: int) -> Tuple[int, int]:
        """使用固定随机数k生成签名"""
        # 计算R = k * G
        R = k * self.G
        
        # 计算e = H(M || ZA)
        e_hash = self.sm2._hash_message(message, public_key)
        e = int.from_bytes(e_hash, 'big') % self.curve.n
        
        # 计算r = (e + x1) mod n
        r = (e + R.x) % self.curve.n
        
        # 计算s = ((1 + d)^-1 * (k - r * d)) mod n
        s = (gmpy2.invert(1 + private_key, self.curve.n) * 
             (k - r * private_key)) % self.curve.n
        
        return r, s
    
    def _recover_private_key_from_nonce_reuse(self, message1: bytes, signature1: Tuple[int, int],
                                            message2: bytes, signature2: Tuple[int, int],
                                            public_key: SM2Point) -> Optional[int]:
        """从随机数重用中恢复私钥"""
        r1, s1 = signature1
        r2, s2 = signature2
        
        # 计算两个消息的哈希值
        e1_hash = self.sm2._hash_message(message1, public_key)
        e1 = int.from_bytes(e1_hash, 'big') % self.curve.n
        
        e2_hash = self.sm2._hash_message(message2, public_key)
        e2 = int.from_bytes(e2_hash, 'big') % self.curve.n
        
        # 由于使用了相同的k，r1 = r2
        if r1 != r2:
            return None
        
        # 建立线性方程组求解私钥
        # s1 = (k - r1 * d) / (1 + d) mod n
        # s2 = (k - r2 * d) / (1 + d) mod n
        # 由于r1 = r2，k相同，可以求解d
        
        # 从s1和s2的关系求解私钥
        # s1 * (1 + d) = k - r1 * d
        # s2 * (1 + d) = k - r2 * d
        # s1 - s2 = (k - r1 * d) / (1 + d) - (k - r2 * d) / (1 + d) = 0
        
        # 实际上，由于r1 = r2，k相同，私钥可以通过以下方式恢复：
        # d = (s1 - s2) / (r1 * (s2 - s1)) mod n
        
        try:
            # 更准确的方法：从s1和s2的关系直接求解
            # s1 = (k - r1 * d) / (1 + d)
            # s2 = (k - r2 * d) / (1 + d)
            # 由于r1 = r2，k相同，实际上s1应该等于s2
            # 如果s1 != s2，说明计算有误
            
            if s1 == s2:
                # 如果s1 = s2，说明两个签名完全相同，无法恢复私钥
                return None
            
            # 尝试从e1, e2, s1, s2的关系恢复私钥
            # 使用更复杂的数学推导
            numerator = (s1 * e2 - s2 * e1) % self.curve.n
            denominator = (r1 * (s2 - s1)) % self.curve.n
            
            if denominator == 0:
                return None
            
            private_key_recovered = (numerator * gmpy2.invert(denominator, self.curve.n)) % self.curve.n
            return private_key_recovered
            
        except Exception as e:
            print(f"Error in private key recovery: {e}")
            return None
    
    def signature_malleability_attack_demo(self):
        """签名可延展性攻击演示"""
        print("\n=== SM2 Signature Malleability Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = b"Message for malleability attack"
        
        # 生成原始签名
        original_signature = self.sm2.sign(message, private_key, public_key)
        print(f"Original signature: (r={original_signature[0]}, s={original_signature[1]})")
        
        # 创建可延展的签名
        malleable_signature = self._create_malleable_signature(original_signature)
        print(f"Malleable signature: (r={malleable_signature[0]}, s={malleable_signature[1]})")
        
        # 验证原始签名
        is_original_valid = self.sm2.verify(message, original_signature, public_key)
        print(f"Original signature verification: {'PASS' if is_original_valid else 'FAIL'}")
        
        # 验证可延展签名
        is_malleable_valid = self.sm2.verify(message, malleable_signature, public_key)
        print(f"Malleable signature verification: {'PASS' if is_malleable_valid else 'FAIL'}")
        
        # 分析签名可延展性
        print(f"\n=== Signature Malleability Analysis ===")
        print("签名可延展性攻击原理：")
        print("1. 攻击者可以修改有效签名而不影响验证结果")
        print("2. 可能导致重放攻击和双重支付问题")
        print("3. 防护措施：在签名中包含消息哈希或使用确定性签名")
        
        return is_malleable_valid
    
    def _create_malleable_signature(self, signature: Tuple[int, int]) -> Tuple[int, int]:
        """创建可延展的签名"""
        r, s = signature
        
        # 简单的可延展性：修改s值
        # 在实际SM2中，可延展性可能更复杂
        malleable_s = (s + 1) % self.curve.n  # 简单的修改示例
        
        return r, malleable_s
    
    def key_recovery_attack_demo(self):
        """密钥恢复攻击演示"""
        print("\n=== SM2 Key Recovery Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        print(f"Original private key: {private_key}")
        print(f"Public key: ({public_key.x}, {public_key.y})")
        
        # 模拟已知部分私钥信息的情况
        # 假设我们知道私钥的某些位
        known_bits = 8  # 假设知道低8位
        known_part = private_key & ((1 << known_bits) - 1)
        
        print(f"Known private key bits: {known_bits}")
        print(f"Known part: {known_part}")
        
        # 尝试恢复完整私钥
        recovered_key = self._recover_private_key_with_partial_info(
            public_key, known_part, known_bits
        )
        
        if recovered_key:
            print(f"Recovered private key: {recovered_key}")
            print(f"Recovery successful: {'YES' if recovered_key == private_key else 'NO'}")
        else:
            print("Private key recovery failed")
        
        # 分析密钥恢复攻击
        print(f"\n=== Key Recovery Attack Analysis ===")
        print("密钥恢复攻击原理：")
        print("1. 利用已知的私钥部分信息")
        print("2. 使用暴力破解或数学方法恢复完整私钥")
        print("3. 防护措施：保护私钥的完整性，使用硬件安全模块")
        
        return recovered_key == private_key
    
    def _recover_private_key_with_partial_info(self, public_key: SM2Point, 
                                             known_part: int, known_bits: int) -> Optional[int]:
        """使用部分私钥信息恢复完整私钥"""
        # 简化的密钥恢复算法
        # 在实际应用中，这需要更复杂的数学方法
        
        mask = (1 << known_bits) - 1
        unknown_bits = 256 - known_bits
        
        # 暴力破解未知部分
        for i in range(min(1000, 1 << min(unknown_bits, 20))):  # 限制搜索空间
            candidate = (i << known_bits) | known_part
            
            if candidate >= self.curve.n:
                continue
            
            # 验证候选私钥
            candidate_public_key = candidate * self.G
            if candidate_public_key.x == public_key.x and candidate_public_key.y == public_key.y:
                return candidate
        
        return None
    
    def comprehensive_attack_demo(self):
        """综合攻击演示"""
        print("\n=== SM2 Comprehensive Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = b"Comprehensive attack test message"
        
        print("Running all attack demonstrations...")
        
        # 1. 重放攻击
        replay_success = self.replay_attack_demo()
        
        # 2. 随机数重用攻击
        nonce_reuse_success = self.nonce_reuse_attack_demo()
        
        # 3. 签名可延展性攻击
        malleability_success = self.signature_malleability_attack_demo()
        
        # 4. 密钥恢复攻击
        key_recovery_success = self.key_recovery_attack_demo()
        
        # 攻击成功率统计
        print(f"\n=== Attack Success Rate Summary ===")
        print(f"Replay Attack: {'SUCCESS' if replay_success else 'FAILED'}")
        print(f"Nonce Reuse Attack: {'SUCCESS' if nonce_reuse_success else 'FAILED'}")
        print(f"Signature Malleability: {'SUCCESS' if malleability_success else 'FAILED'}")
        print(f"Key Recovery Attack: {'SUCCESS' if key_recovery_success else 'FAILED'}")
        
        # 防护建议
        print(f"\n=== Security Recommendations ===")
        print("1. 使用安全的随机数生成器")
        print("2. 在签名中包含时间戳或随机数")
        print("3. 验证签名的唯一性")
        print("4. 使用硬件安全模块保护私钥")
        print("5. 定期更新密钥对")
        print("6. 实施签名验证的完整性检查")
        
        return {
            'replay': replay_success,
            'nonce_reuse': nonce_reuse_success,
            'malleability': malleability_success,
            'key_recovery': key_recovery_success
        }


def main():
    """主函数：运行所有攻击演示"""
    print("=== SM2 Signature Algorithm Misuse POC ===")
    print("基于20250713-wen-sm2-public.pdf的签名算法误用分析")
    
    # 创建攻击演示实例
    misuse_attacks = SM2SignatureMisuse()
    
    # 运行综合攻击演示
    results = misuse_attacks.comprehensive_attack_demo()
    
    print(f"\n=== Final Results ===")
    success_count = sum(results.values())
    total_count = len(results)
    print(f"Successful attacks: {success_count}/{total_count}")
    print(f"Success rate: {success_count/total_count*100:.1f}%")


if __name__ == "__main__":
    main() 