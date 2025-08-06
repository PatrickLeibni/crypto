#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enhanced SM2 Signature Algorithm Misuse POC
增强版SM2签名算法误用攻击验证

This module implements comprehensive signature algorithm misuse attacks on SM2
including all attack types mentioned in the requirements
"""

import hashlib
import os
import random
import time
from typing import Tuple, List, Dict, Optional
import gmpy2
from gmpy2 import mpz
from sm2_basic import SM2, SM2Point, SM2Curve


class EnhancedSM2SignatureMisuse:
    """增强版SM2签名算法误用攻击类"""
    
    def __init__(self):
        self.sm2 = SM2()
        self.curve = self.sm2.curve
        self.G = self.sm2.G
    
    def k_leakage_attack_demo(self):
        """泄露随机数k导致私钥泄露攻击演示"""
        print("=== SM2 Random Number k Leakage Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = b"Message for k leakage attack"
        
        # 生成随机数k（模拟泄露）
        k = random.randint(1, self.curve.n - 1)
        
        # 使用固定k生成签名
        signature = self._sign_with_fixed_k(message, private_key, public_key, k)
        print(f"Message: {message}")
        print(f"Signature: (r={signature[0]}, s={signature[1]})")
        print(f"Leaked k: {k}")
        
        # 验证签名
        is_valid = self.sm2.verify(message, signature, public_key)
        print(f"Signature verification: {'PASS' if is_valid else 'FAIL'}")
        
        # 从泄露的k恢复私钥
        recovered_private_key = self._recover_private_key_from_k_leakage(
            signature, k, message, public_key
        )
        
        if recovered_private_key:
            print(f"Recovered private key: {recovered_private_key}")
            print(f"Original private key: {private_key}")
            print(f"Recovery successful: {'YES' if recovered_private_key == private_key else 'NO'}")
        else:
            print("Private key recovery failed")
        
        return recovered_private_key == private_key
    
    def _recover_private_key_from_k_leakage(self, signature: Tuple[int, int], k: int,
                                          message: bytes, public_key: SM2Point) -> Optional[int]:
        """从泄露的k恢复私钥"""
        r, s = signature
        
        # 计算消息哈希
        e_hash = self.sm2._hash_message(message, public_key)
        e = int.from_bytes(e_hash, 'big') % self.curve.n
        
        # 使用公式: d_A = (s + r)^(-1) * (k - s) mod n
        try:
            # 计算 (s + r)^(-1)
            s_plus_r_inv = gmpy2.invert(s + r, self.curve.n)
            
            # 计算 (k - s)
            k_minus_s = (k - s) % self.curve.n
            
            # 计算私钥
            private_key = (s_plus_r_inv * k_minus_s) % self.curve.n
            
            return private_key
        except Exception as e:
            print(f"Error in private key recovery: {e}")
            return None
    
    def same_user_nonce_reuse_attack_demo(self):
        """同一用户重复使用随机数k导致私钥泄露攻击演示"""
        print("\n=== SM2 Same User Nonce Reuse Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 模拟重复使用k
        reused_k = random.randint(1, self.curve.n - 1)
        
        # 使用相同k对不同消息签名
        message1 = b"First message with reused k"
        message2 = b"Second message with reused k"
        
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
        
        # 从重复使用k恢复私钥
        recovered_private_key = self._recover_private_key_from_same_user_nonce_reuse(
            message1, signature1, message2, signature2, public_key
        )
        
        if recovered_private_key:
            print(f"Recovered private key: {recovered_private_key}")
            print(f"Original private key: {private_key}")
            print(f"Recovery successful: {'YES' if recovered_private_key == private_key else 'NO'}")
        else:
            print("Private key recovery failed")
        
        return recovered_private_key == private_key
    
    def _recover_private_key_from_same_user_nonce_reuse(self, message1: bytes, signature1: Tuple[int, int],
                                                       message2: bytes, signature2: Tuple[int, int],
                                                       public_key: SM2Point) -> Optional[int]:
        """从同一用户重复使用k恢复私钥"""
        r1, s1 = signature1
        r2, s2 = signature2
        
        # 计算消息哈希
        e1_hash = self.sm2._hash_message(message1, public_key)
        e1 = int.from_bytes(e1_hash, 'big') % self.curve.n
        
        e2_hash = self.sm2._hash_message(message2, public_key)
        e2 = int.from_bytes(e2_hash, 'big') % self.curve.n
        
        # 使用公式: d_A = (s2 - s1) / (s1 - s2 + r1 - r2) mod n
        try:
            numerator = (s2 - s1) % self.curve.n
            denominator = (s1 - s2 + r1 - r2) % self.curve.n
            
            if denominator == 0:
                return None
            
            private_key = (numerator * gmpy2.invert(denominator, self.curve.n)) % self.curve.n
            return private_key
        except Exception as e:
            print(f"Error in private key recovery: {e}")
            return None
    
    def different_users_same_k_attack_demo(self):
        """不同用户使用相同k导致私钥泄露攻击演示"""
        print("\n=== SM2 Different Users Same k Attack Demo ===")
        
        # 生成两个用户的密钥对
        private_key_alice, public_key_alice = self.sm2.generate_keypair()
        private_key_bob, public_key_bob = self.sm2.generate_keypair()
        
        # 模拟使用相同的k
        shared_k = random.randint(1, self.curve.n - 1)
        
        # Alice和Bob使用相同k签名
        message_alice = b"Alice's message with shared k"
        message_bob = b"Bob's message with shared k"
        
        signature_alice = self._sign_with_fixed_k(message_alice, private_key_alice, public_key_alice, shared_k)
        signature_bob = self._sign_with_fixed_k(message_bob, private_key_bob, public_key_bob, shared_k)
        
        print(f"Alice's message: {message_alice}")
        print(f"Alice's signature: (r={signature_alice[0]}, s={signature_alice[1]})")
        print(f"Bob's message: {message_bob}")
        print(f"Bob's signature: (r={signature_bob[0]}, s={signature_bob[1]})")
        print(f"Shared k: {shared_k}")
        
        # 验证签名
        is_alice_valid = self.sm2.verify(message_alice, signature_alice, public_key_alice)
        is_bob_valid = self.sm2.verify(message_bob, signature_bob, public_key_bob)
        print(f"Alice's signature verification: {'PASS' if is_alice_valid else 'FAIL'}")
        print(f"Bob's signature verification: {'PASS' if is_bob_valid else 'FAIL'}")
        
        # 尝试恢复私钥
        recovered_alice_key = self._recover_private_key_from_different_users_same_k(
            signature_alice, shared_k, message_alice, public_key_alice
        )
        recovered_bob_key = self._recover_private_key_from_different_users_same_k(
            signature_bob, shared_k, message_bob, public_key_bob
        )
        
        if recovered_alice_key:
            print(f"Recovered Alice's private key: {recovered_alice_key}")
            print(f"Original Alice's private key: {private_key_alice}")
            print(f"Alice's key recovery: {'YES' if recovered_alice_key == private_key_alice else 'NO'}")
        
        if recovered_bob_key:
            print(f"Recovered Bob's private key: {recovered_bob_key}")
            print(f"Original Bob's private key: {private_key_bob}")
            print(f"Bob's key recovery: {'YES' if recovered_bob_key == private_key_bob else 'NO'}")
        
        return (recovered_alice_key == private_key_alice) or (recovered_bob_key == private_key_bob)
    
    def _recover_private_key_from_different_users_same_k(self, signature: Tuple[int, int], k: int,
                                                        message: bytes, public_key: SM2Point) -> Optional[int]:
        """从不同用户使用相同k恢复私钥"""
        r, s = signature
        
        # 使用公式: d_B = (k - s2) / (s2 + r2) mod n
        try:
            numerator = (k - s) % self.curve.n
            denominator = (s + r) % self.curve.n
            
            if denominator == 0:
                return None
            
            private_key = (numerator * gmpy2.invert(denominator, self.curve.n)) % self.curve.n
            return private_key
        except Exception as e:
            print(f"Error in private key recovery: {e}")
            return None
    
    def sm2_ecdsa_same_key_attack_demo(self):
        """SM2与ECDSA使用相同私钥d和随机数k导致私钥泄露攻击演示"""
        print("\n=== SM2-ECDSA Same Key Attack Demo ===")
        
        # 生成密钥对（SM2和ECDSA使用相同私钥）
        private_key, public_key = self.sm2.generate_keypair()
        
        # 模拟使用相同的k
        shared_k = random.randint(1, self.curve.n - 1)
        
        # 生成SM2签名
        message = b"Message for SM2-ECDSA attack"
        sm2_signature = self._sign_with_fixed_k(message, private_key, public_key, shared_k)
        
        # 模拟ECDSA签名（使用相同私钥和k）
        ecdsa_signature = self._simulate_weak_ecdsa_signature(message, private_key, shared_k)
        
        print(f"Message: {message}")
        print(f"SM2 signature: (r={sm2_signature[0]}, s={sm2_signature[1]})")
        print(f"ECDSA signature: (r={ecdsa_signature[0]}, s={ecdsa_signature[1]})")
        print(f"Shared k: {shared_k}")
        
        # 验证SM2签名
        is_sm2_valid = self.sm2.verify(message, sm2_signature, public_key)
        print(f"SM2 signature verification: {'PASS' if is_sm2_valid else 'FAIL'}")
        
        # 从两个签名恢复私钥
        recovered_private_key = self._recover_private_key_from_sm2_ecdsa_same_key(
            sm2_signature, ecdsa_signature, message, public_key
        )
        
        if recovered_private_key:
            print(f"Recovered private key: {recovered_private_key}")
            print(f"Original private key: {private_key}")
            print(f"Recovery successful: {'YES' if recovered_private_key == private_key else 'NO'}")
        else:
            print("Private key recovery failed")
        
        return recovered_private_key == private_key
    
    def _simulate_ecdsa_signature(self, message: bytes, private_key: int, k: int) -> Tuple[int, int]:
        """模拟ECDSA签名"""
        # 计算R = k * G
        R = k * self.G
        
        # 计算消息哈希
        e_hash = hashlib.sha256(message).digest()
        e = int.from_bytes(e_hash, 'big') % self.curve.n
        
        # ECDSA签名计算
        r = R.x % self.curve.n
        s = (gmpy2.invert(k, self.curve.n) * (e + r * private_key)) % self.curve.n
        
        return r, s
    
    def _simulate_weak_ecdsa_signature(self, message: bytes, private_key: int, k: int) -> Tuple[int, int]:
        """模拟弱ECDSA签名（用于攻击演示）"""
        # 计算R = k * G
        R = k * self.G
        
        # 计算消息哈希
        e_hash = hashlib.sha256(message).digest()
        e = int.from_bytes(e_hash, 'big') % self.curve.n
        
        # 弱ECDSA签名计算（故意使用相同的k）
        r = R.x % self.curve.n
        s = (gmpy2.invert(k, self.curve.n) * (e + r * private_key)) % self.curve.n
        
        return r, s
    
    def _recover_private_key_from_sm2_ecdsa_same_key(self, sm2_signature: Tuple[int, int],
                                                     ecdsa_signature: Tuple[int, int],
                                                     message: bytes, public_key: SM2Point) -> Optional[int]:
        """从SM2和ECDSA使用相同密钥恢复私钥"""
        r1, s1 = sm2_signature  # SM2签名
        r2, s2 = ecdsa_signature  # ECDSA签名
        
        # 计算消息哈希
        e_hash = hashlib.sha256(message).digest()
        e = int.from_bytes(e_hash, 'big') % self.curve.n
        
        # 更准确的数学推导
        # SM2: s1 = (k - r1 * d) / (1 + d) mod n
        # ECDSA: s2 = (e + r2 * d) / k mod n
        # 从这两个方程可以推导出私钥
        try:
            # 从SM2方程: k = s1 * (1 + d) + r1 * d
            # 从ECDSA方程: k = (e + r2 * d) / s2
            # 联立: s1 * (1 + d) + r1 * d = (e + r2 * d) / s2
            # 整理: s1 * s2 * (1 + d) + r1 * s2 * d = e + r2 * d
            # 进一步: s1 * s2 + s1 * s2 * d + r1 * s2 * d = e + r2 * d
            # 最终: d = (e - s1 * s2) / (s1 * s2 + r1 * s2 - r2) mod n
            
            # 尝试多种推导方法
            methods = [
                # 方法1: 原始公式
                lambda: ((e - s1 * s2) * gmpy2.invert(s1 * s2 + r1 * s2 - r2, self.curve.n)) % self.curve.n,
                # 方法2: 简化公式
                lambda: ((s1 * s2 - e) * gmpy2.invert(r2 - s1 * s2 - r1 * s2, self.curve.n)) % self.curve.n,
                # 方法3: 直接求解
                lambda: ((k - s1) * gmpy2.invert(s1 + r1, self.curve.n)) % self.curve.n
            ]
            
            for i, method in enumerate(methods):
                try:
                    private_key_candidate = method()
                    # 验证候选私钥
                    candidate_public_key = private_key_candidate * self.G
                    if candidate_public_key.x == public_key.x and candidate_public_key.y == public_key.y:
                        print(f"Method {i+1} succeeded")
                        return private_key_candidate
                except Exception:
                    continue
            
            return None
        except Exception as e:
            print(f"Error in private key recovery: {e}")
            return None
    
    def signature_verification_bypass_attack_demo(self):
        """签名验证未检查消息等导致的问题演示"""
        print("\n=== SM2 Signature Verification Bypass Attack Demo ===")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 生成原始签名
        original_message = b"Original message for verification bypass"
        original_signature = self.sm2.sign(original_message, private_key, public_key)
        
        print(f"Original message: {original_message}")
        print(f"Original signature: (r={original_signature[0]}, s={original_signature[1]})")
        
        # 验证原始签名
        is_original_valid = self.sm2.verify(original_message, original_signature, public_key)
        print(f"Original signature verification: {'PASS' if is_original_valid else 'FAIL'}")
        
        # 模拟弱签名验证（不检查消息内容）
        different_messages = [
            b"Different message 1",
            b"Different message 2", 
            b"Completely different message",
            b"Attack message"
        ]
        
        # 模拟弱验证器（故意不检查消息）
        bypass_success = self._simulate_weak_verification(different_messages, original_signature, public_key)
        
        # 分析验证绕过攻击
        print(f"\n=== Verification Bypass Analysis ===")
        print("签名验证绕过攻击原理：")
        print("1. 如果签名验证时未检查消息内容")
        print("2. 攻击者可以使用有效签名验证任意消息")
        print("3. 这可能导致伪造签名的风险")
        print("4. 防护措施：严格验证签名与消息的对应关系")
        
        return bypass_success
    
    def _simulate_weak_verification(self, messages: List[bytes], signature: Tuple[int, int], 
                                   public_key: SM2Point) -> bool:
        """模拟弱签名验证器（不检查消息内容）"""
        print("\nSimulating weak signature verification (not checking message content)...")
        
        bypass_count = 0
        for i, message in enumerate(messages):
            # 模拟弱验证器：只检查签名格式，不检查消息内容
            r, s = signature
            
            # 检查签名格式是否有效
            if 0 < r < self.curve.n and 0 < s < self.curve.n:
                # 弱验证器认为这是有效的
                print(f"Weak verification {i+1} ({message}): SUCCESS (format valid)")
                bypass_count += 1
            else:
                print(f"Weak verification {i+1} ({message}): FAIL (format invalid)")
        
        print(f"Bypass success rate: {bypass_count}/{len(messages)}")
        return bypass_count > 0
    
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
    
    def comprehensive_enhanced_attack_demo(self):
        """综合增强攻击演示"""
        print("\n=== SM2 Comprehensive Enhanced Attack Demo ===")
        
        print("Running all enhanced attack demonstrations...")
        
        # 1. 泄露随机数k攻击
        print("\n" + "="*50)
        print("1. 泄露随机数k导致私钥泄露攻击验证")
        print("="*50)
        k_leakage_success = self.k_leakage_attack_demo()
        print(f"验证结果: {'成功' if k_leakage_success else '失败'}")
        
        # 2. 同一用户重复使用k攻击
        print("\n" + "="*50)
        print("2. 同一用户重复使用随机数k导致私钥泄露攻击验证")
        print("="*50)
        same_user_nonce_reuse_success = self.same_user_nonce_reuse_attack_demo()
        print(f"验证结果: {'成功' if same_user_nonce_reuse_success else '失败'}")
        
        # 3. 不同用户使用相同k攻击
        print("\n" + "="*50)
        print("3. 不同用户使用相同k导致私钥泄露攻击验证")
        print("="*50)
        different_users_same_k_success = self.different_users_same_k_attack_demo()
        print(f"验证结果: {'成功' if different_users_same_k_success else '失败'}")
        
        # 4. SM2与ECDSA使用相同密钥攻击
        print("\n" + "="*50)
        print("4. SM2与ECDSA使用相同私钥d和随机数k导致私钥泄露攻击验证")
        print("="*50)
        sm2_ecdsa_same_key_success = self.sm2_ecdsa_same_key_attack_demo()
        print(f"验证结果: {'成功' if sm2_ecdsa_same_key_success else '失败'}")
        
        # 5. 签名验证绕过攻击
        print("\n" + "="*50)
        print("5. 签名验证未检查消息等导致的问题验证")
        print("="*50)
        verification_bypass_success = self.signature_verification_bypass_attack_demo()
        print(f"验证结果: {'成功' if verification_bypass_success else '失败'}")
        
        # 攻击成功率统计
        print(f"\n" + "="*60)
        print("=== 增强攻击验证结果汇总 ===")
        print("="*60)
        print(f"1. 泄露随机数k攻击: {'成功' if k_leakage_success else '失败'}")
        print(f"2. 同一用户重复使用k攻击: {'成功' if same_user_nonce_reuse_success else '失败'}")
        print(f"3. 不同用户使用相同k攻击: {'成功' if different_users_same_k_success else '失败'}")
        print(f"4. SM2-ECDSA使用相同密钥攻击: {'成功' if sm2_ecdsa_same_key_success else '失败'}")
        print(f"5. 签名验证绕过攻击: {'成功' if verification_bypass_success else '失败'}")
        
        success_count = sum([k_leakage_success, same_user_nonce_reuse_success, 
                           different_users_same_k_success, sm2_ecdsa_same_key_success, 
                           verification_bypass_success])
        total_count = 5
        print(f"\n总成功率: {success_count}/{total_count} ({success_count/total_count*100:.1f}%)")
        
        # 防护建议
        print(f"\n" + "="*60)
        print("=== 安全防护建议 ===")
        print("="*60)
        print("1. 严格保护随机数k，确保不泄露")
        print("2. 每次签名使用不同的随机数k")
        print("3. 不同用户使用不同的随机数生成器")
        print("4. 避免SM2和ECDSA使用相同的私钥和随机数")
        print("5. 实施严格的签名验证，检查消息内容")
        print("6. 使用安全的随机数生成器")
        print("7. 定期更新密钥对")
        print("8. 实施签名验证的完整性检查")
        
        return {
            'k_leakage': k_leakage_success,
            'same_user_nonce_reuse': same_user_nonce_reuse_success,
            'different_users_same_k': different_users_same_k_success,
            'sm2_ecdsa_same_key': sm2_ecdsa_same_key_success,
            'verification_bypass': verification_bypass_success
        }


def main():
    """主函数：运行所有增强攻击演示"""
    print("="*70)
    print("增强版SM2签名算法误用POC验证")
    print("Enhanced SM2 Signature Algorithm Misuse POC Verification")
    print("="*70)
    print("基于20250713-wen-sm2-public.pdf的增强签名算法误用分析")
    print("包含以下5种攻击类型的POC验证：")
    print("1. 泄露随机数k导致私钥泄露")
    print("2. 同一用户重复使用随机数k")
    print("3. 不同用户使用相同k")
    print("4. SM2与ECDSA使用相同私钥d和随机数k")
    print("5. 签名验证未检查消息等导致的问题")
    print("="*70)
    
    # 创建增强攻击演示实例
    enhanced_misuse_attacks = EnhancedSM2SignatureMisuse()
    
    # 运行综合增强攻击演示
    results = enhanced_misuse_attacks.comprehensive_enhanced_attack_demo()
    
    print(f"\n" + "="*70)
    print("=== 最终验证结果 ===")
    print("="*70)
    success_count = sum(results.values())
    total_count = len(results)
    print(f"成功验证的攻击类型: {success_count}/{total_count}")
    print(f"验证成功率: {success_count/total_count*100:.1f}%")
    
    if success_count > 0:
        print(f"\n成功验证的攻击类型详情:")
        for attack_name, success in results.items():
            if success:
                print(f"✓ {attack_name}")
    
    print(f"\n" + "="*70)
    print("验证完成！")
    print("="*70)


if __name__ == "__main__":
    main() 