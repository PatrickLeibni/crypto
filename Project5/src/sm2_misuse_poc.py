#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2 Signature Algorithm Misuse POC
基于20250713-wen-sm2-public.pdf的签名算法误用POC验证

This module implements various signature algorithm misuse attacks on SM2
based on the analysis in the mentioned PDF document.
"""

import hashlib
import os
import random
import time
from typing import Tuple, List, Dict, Optional
from .sm2_simple import SimpleSM2


class SM2MisusePOC:
    """SM2签名算法误用POC验证类"""
    
    def __init__(self):
        self.sm2 = SimpleSM2()
        self.curve = self.sm2
        self.G = (self.sm2.Gx, self.sm2.Gy)
    
    def replay_attack_demo(self):
        """重放攻击演示"""
        print("=== SM2重放攻击演示 ===")
        print("基于20250713-wen-sm2-public.pdf中的重放攻击分析")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = "原始消息用于重放攻击".encode('utf-8')
        
        # 生成原始签名
        original_signature = self.sm2.sign(message, private_key, public_key)
        print(f"原始签名: (r={original_signature[0]}, s={original_signature[1]})")
        
        # 验证原始签名
        is_valid = self.sm2.verify(message, original_signature, public_key)
        print(f"原始签名验证: {'通过' if is_valid else '失败'}")
        
        # 重放攻击：使用相同的签名验证不同的消息
        different_message = "不同的消息用于重放攻击".encode('utf-8')
        is_replay_valid = self.sm2.verify(different_message, original_signature, public_key)
        print(f"重放攻击验证: {'成功' if is_replay_valid else '失败'}")
        
        # 分析重放攻击的原理
        print(f"\n=== 重放攻击分析 ===")
        print("重放攻击原理：")
        print("1. 攻击者截获有效的SM2签名")
        print("2. 将签名重放到不同的消息上")
        print("3. 如果签名验证通过，说明存在重放漏洞")
        print("4. 防护措施：在签名中包含时间戳或随机数")
        
        return is_replay_valid
    
    def nonce_reuse_attack_demo(self):
        """随机数重用攻击演示"""
        print("\n=== SM2随机数重用攻击演示 ===")
        print("基于20250713-wen-sm2-public.pdf中的随机数重用分析")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        
        # 模拟随机数重用
        reused_k = random.randint(1, self.curve.n - 1)
        
        # 使用相同随机数k生成两个不同消息的签名
        message1 = "使用重用随机数的第一个消息".encode('utf-8')
        message2 = "使用重用随机数的第二个消息".encode('utf-8')
        
        # 手动生成签名（模拟随机数重用）
        signature1 = self._sign_with_fixed_k(message1, private_key, public_key, reused_k)
        signature2 = self._sign_with_fixed_k(message2, private_key, public_key, reused_k)
        
        print(f"消息1: {message1}")
        print(f"签名1: (r={signature1[0]}, s={signature1[1]})")
        print(f"消息2: {message2}")
        print(f"签名2: (r={signature2[0]}, s={signature2[1]})")
        
        # 验证签名
        is_valid1 = self.sm2.verify(message1, signature1, public_key)
        is_valid2 = self.sm2.verify(message2, signature2, public_key)
        print(f"签名1验证: {'通过' if is_valid1 else '失败'}")
        print(f"签名2验证: {'通过' if is_valid2 else '失败'}")
        
        # 尝试从重用随机数中恢复私钥
        recovered_private_key = self._recover_private_key_from_nonce_reuse(
            message1, signature1, message2, signature2, public_key
        )
        
        if recovered_private_key:
            print(f"恢复的私钥: {recovered_private_key}")
            print(f"原始私钥: {private_key}")
            print(f"恢复成功: {'是' if recovered_private_key == private_key else '否'}")
        else:
            print("私钥恢复失败")
        
        # 分析随机数重用攻击
        print(f"\n=== 随机数重用攻击分析 ===")
        print("随机数重用攻击原理：")
        print("1. 如果两个签名使用相同的随机数k")
        print("2. 可以建立线性方程组求解私钥")
        print("3. 防护措施：确保每次签名使用不同的随机数")
        
        return recovered_private_key == private_key
    
    def _sign_with_fixed_k(self, message: bytes, private_key: int, 
                          public_key: Tuple[int, int], k: int) -> Tuple[int, int]:
        """使用固定随机数k生成签名"""
        # 计算R = k * G
        R = self.sm2._scalar_multiply(k, (self.sm2.Gx, self.sm2.Gy))
        
        # 计算e = H(M || ZA)
        e = self.sm2._hash_message(message, public_key)
        
        # 计算r = (e + x1) mod n
        r = (e + R[0]) % self.sm2.n
        
        # 计算s = ((1 + d)^-1 * (k - r * d)) mod n
        s = (self.sm2._mod_inverse(1 + private_key, self.sm2.n) * 
             (k - r * private_key)) % self.sm2.n
        
        return r, s
    
    def _recover_private_key_from_nonce_reuse(self, message1: bytes, signature1: Tuple[int, int],
                                            message2: bytes, signature2: Tuple[int, int],
                                            public_key: Tuple[int, int]) -> Optional[int]:
        """从随机数重用中恢复私钥"""
        r1, s1 = signature1
        r2, s2 = signature2
        
        # 计算两个消息的哈希值
        e1 = self.sm2._hash_message(message1, public_key)
        e2 = self.sm2._hash_message(message2, public_key)
        
        # 由于使用了相同的k，r1 = r2
        if r1 != r2:
            return None
        
        # 建立线性方程组求解私钥
        # s1 = (k - r1 * d) / (1 + d) mod n
        # s2 = (k - r2 * d) / (1 + d) mod n
        # 由于r1 = r2，k相同，可以求解d
        
        try:
            # 从s1和s2的关系求解私钥
            # s1 * (1 + d) = k - r1 * d
            # s2 * (1 + d) = k - r2 * d
            # s1 - s2 = (k - r1 * d) / (1 + d) - (k - r2 * d) / (1 + d) = 0
            
            # 实际上，由于r1 = r2，k相同，私钥可以通过以下方式恢复：
            # d = (s1 - s2) / (r1 * (s2 - s1)) mod n
            
            if s1 == s2:
                # 如果s1 = s2，说明两个签名完全相同，无法恢复私钥
                return None
            
            # 尝试从e1, e2, s1, s2的关系恢复私钥
            numerator = (s1 * e2 - s2 * e1) % self.sm2.n
            denominator = (r1 * (s2 - s1)) % self.sm2.n
            
            if denominator == 0:
                return None
            
            private_key_recovered = (numerator * self.sm2._mod_inverse(denominator, self.sm2.n)) % self.sm2.n
            return private_key_recovered
            
        except Exception as e:
            print(f"私钥恢复错误: {e}")
            return None
    
    def signature_malleability_attack_demo(self):
        """签名可延展性攻击演示"""
        print("\n=== SM2签名可延展性攻击演示 ===")
        print("基于20250713-wen-sm2-public.pdf中的签名可延展性分析")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = "用于可延展性攻击的消息".encode('utf-8')
        
        # 生成原始签名
        original_signature = self.sm2.sign(message, private_key, public_key)
        print(f"原始签名: (r={original_signature[0]}, s={original_signature[1]})")
        
        # 创建可延展的签名
        malleable_signature = self._create_malleable_signature(original_signature)
        print(f"可延展签名: (r={malleable_signature[0]}, s={malleable_signature[1]})")
        
        # 验证原始签名
        is_original_valid = self.sm2.verify(message, original_signature, public_key)
        print(f"原始签名验证: {'通过' if is_original_valid else '失败'}")
        
        # 验证可延展签名
        is_malleable_valid = self.sm2.verify(message, malleable_signature, public_key)
        print(f"可延展签名验证: {'通过' if is_malleable_valid else '失败'}")
        
        # 分析签名可延展性
        print(f"\n=== 签名可延展性分析 ===")
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
        malleable_s = (s + 1) % self.sm2.n  # 简单的修改示例
        
        return r, malleable_s
    
    def key_recovery_attack_demo(self):
        """密钥恢复攻击演示"""
        print("\n=== SM2密钥恢复攻击演示 ===")
        print("基于20250713-wen-sm2-public.pdf中的密钥恢复分析")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        print(f"原始私钥: {private_key}")
        print(f"公钥: ({public_key[0]}, {public_key[1]})")
        
        # 模拟已知部分私钥信息的情况
        # 假设我们知道私钥的某些位
        known_bits = 8  # 假设知道低8位
        known_part = private_key & ((1 << known_bits) - 1)
        
        print(f"已知私钥位数: {known_bits}")
        print(f"已知部分: {known_part}")
        
        # 尝试恢复完整私钥
        recovered_key = self._recover_private_key_with_partial_info(
            public_key, known_part, known_bits
        )
        
        if recovered_key:
            print(f"恢复的私钥: {recovered_key}")
            print(f"恢复成功: {'是' if recovered_key == private_key else '否'}")
        else:
            print("私钥恢复失败")
        
        # 分析密钥恢复攻击
        print(f"\n=== 密钥恢复攻击分析 ===")
        print("密钥恢复攻击原理：")
        print("1. 利用已知的私钥部分信息")
        print("2. 使用暴力破解或数学方法恢复完整私钥")
        print("3. 防护措施：保护私钥的完整性，使用硬件安全模块")
        
        return recovered_key == private_key
    
    def _recover_private_key_with_partial_info(self, public_key: Tuple[int, int], 
                                             known_part: int, known_bits: int) -> Optional[int]:
        """使用部分私钥信息恢复完整私钥"""
        # 简化的密钥恢复算法
        # 在实际应用中，这需要更复杂的数学方法
        
        mask = (1 << known_bits) - 1
        unknown_bits = 256 - known_bits
        
        # 暴力破解未知部分
        for i in range(min(1000, 1 << min(unknown_bits, 20))):  # 限制搜索空间
            candidate = (i << known_bits) | known_part
            
            if candidate >= self.sm2.n:
                continue
            
            # 验证候选私钥
            candidate_public_key = self.sm2._scalar_multiply(candidate, (self.sm2.Gx, self.sm2.Gy))
            if candidate_public_key and candidate_public_key[0] == public_key[0] and candidate_public_key[1] == public_key[1]:
                return candidate
        
        return None
    
    def comprehensive_attack_demo(self):
        """综合攻击演示"""
        print("\n=== SM2综合攻击演示 ===")
        print("基于20250713-wen-sm2-public.pdf的完整攻击分析")
        
        # 生成密钥对
        private_key, public_key = self.sm2.generate_keypair()
        message = "综合攻击测试消息".encode('utf-8')
        
        print("运行所有攻击演示...")
        
        # 1. 重放攻击
        replay_success = self.replay_attack_demo()
        
        # 2. 随机数重用攻击
        nonce_reuse_success = self.nonce_reuse_attack_demo()
        
        # 3. 签名可延展性攻击
        malleability_success = self.signature_malleability_attack_demo()
        
        # 4. 密钥恢复攻击
        key_recovery_success = self.key_recovery_attack_demo()
        
        # 攻击成功率统计
        print(f"\n=== 攻击成功率统计 ===")
        print(f"重放攻击: {'成功' if replay_success else '失败'}")
        print(f"随机数重用攻击: {'成功' if nonce_reuse_success else '失败'}")
        print(f"签名可延展性: {'成功' if malleability_success else '失败'}")
        print(f"密钥恢复攻击: {'成功' if key_recovery_success else '失败'}")
        
        # 防护建议
        print(f"\n=== 安全建议 ===")
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
    print("=== SM2签名算法误用POC验证 ===")
    print("基于20250713-wen-sm2-public.pdf的签名算法误用分析")
    
    # 创建攻击演示实例
    misuse_poc = SM2MisusePOC()
    
    # 运行综合攻击演示
    results = misuse_poc.comprehensive_attack_demo()
    
    print(f"\n=== 最终结果 ===")
    success_count = sum(results.values())
    total_count = len(results)
    print(f"成功攻击: {success_count}/{total_count}")
    print(f"成功率: {success_count/total_count*100:.1f}%")


if __name__ == "__main__":
    main() 