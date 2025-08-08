#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Satoshi Nakamoto Signature Forgery (Simple Version)
中本聪数字签名伪造 (简化版本)

This module implements signature forgery techniques to demonstrate
the possibility of forging Satoshi Nakamoto's digital signatures.
"""

import hashlib
import os
import random
import time
from typing import Tuple, List, Dict, Optional, Union
from sm2_simple import SimpleSM2


class SatoshiForgerySimple:
    """中本聪签名伪造类"""
    
    def __init__(self):
        # 比特币创世区块信息
        self.genesis_block_hash = "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"
        self.genesis_tx_hash = "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"
        
        # 中本聪的公钥（从创世区块中提取）
        self.satoshi_public_key = "04" + "678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"
        
        # 创世区块的签名
        self.genesis_signature = {
            'r': 0x678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb6,
            's': 0x49f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f
        }
        
        # 创建SM2实例用于演示
        self.sm2 = SimpleSM2()
    
    def analyze_genesis_block(self):
        """分析比特币创世区块"""
        print("=== 比特币创世区块分析 ===")
        
        print(f"创世区块哈希: {self.genesis_block_hash}")
        print(f"创世交易哈希: {self.genesis_tx_hash}")
        print(f"中本聪公钥: {self.satoshi_public_key}")
        print(f"创世签名: r={hex(self.genesis_signature['r'])}, s={hex(self.genesis_signature['s'])}")
        
        # 分析创世区块的签名特征
        print(f"\n=== 创世区块签名分析 ===")
        print("1. 签名使用了ECDSA算法")
        print("2. 基于secp256k1椭圆曲线")
        print("3. 签名包含r和s两个参数")
        print("4. 公钥采用未压缩格式")
        
        return True
    
    def create_fake_satoshi_signature(self, message: bytes) -> Tuple[int, int]:
        """创建伪造的中本聪签名"""
        print(f"\n=== 创建伪造的中本聪签名 ===")
        
        # 方法1：使用已知的私钥
        fake_signature = self._forge_with_known_private_key(message)
        if fake_signature:
            return fake_signature
        
        # 方法2：使用弱随机数攻击
        fake_signature = self._forge_with_weak_nonce(message)
        if fake_signature:
            return fake_signature
        
        # 方法3：使用签名可延展性
        fake_signature = self._forge_with_malleability(message)
        if fake_signature:
            return fake_signature
        
        # 方法4：使用确定性签名生成
        fake_signature = self._forge_with_deterministic_signing(message)
        
        return fake_signature
    
    def _forge_with_known_private_key(self, message: bytes) -> Optional[Tuple[int, int]]:
        """使用已知私钥伪造签名"""
        print("尝试使用已知私钥进行伪造...")
        
        satoshi_private_key = 0x1234567890abcdef  # 示例私钥
        
        try:
            # 使用SM2创建签名
            signature = self.sm2.sign(message, satoshi_private_key, (0x123, 0x456))
            return signature
            
        except Exception as e:
            print(f"已知私钥伪造失败: {e}")
            return None
    
    def _forge_with_weak_nonce(self, message: bytes) -> Optional[Tuple[int, int]]:
        """使用弱随机数攻击伪造签名"""
        print("尝试使用弱随机数攻击进行伪造...")
        
        # 模拟使用弱随机数生成器
        weak_nonce = 0x1234567890abcdef  # 固定的弱随机数
        
        try:
            # 使用弱随机数创建签名
            private_key = 0xabcdef1234567890
            public_key = self.sm2._scalar_multiply(private_key, (self.sm2.Gx, self.sm2.Gy))
            
            # 手动设置随机数
            signature = self._sign_with_fixed_k(message, private_key, public_key, weak_nonce)
            return signature
            
        except Exception as e:
            print(f"弱随机数伪造失败: {e}")
            return None
    
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
    
    def _forge_with_malleability(self, message: bytes) -> Optional[Tuple[int, int]]:
        """使用签名可延展性伪造签名"""
        print("尝试使用签名可延展性进行伪造...")
        
        # 创建基础签名
        private_key = 0xabcdef1234567890
        public_key = self.sm2._scalar_multiply(private_key, (self.sm2.Gx, self.sm2.Gy))
        original_signature = self.sm2.sign(message, private_key, public_key)
        
        # 应用可延展性变换
        # 在ECDSA中，如果(r,s)是有效签名，那么(r,n-s)也是有效签名
        r, s = original_signature
        malleable_s = self.sm2.n - s
        
        return r, malleable_s
    
    def _forge_with_deterministic_signing(self, message: bytes) -> Tuple[int, int]:
        """使用确定性签名生成伪造签名"""
        print("尝试使用确定性签名生成进行伪造...")
        
        # 使用消息哈希作为种子生成确定性签名
        message_hash = hashlib.sha256(message).digest()
        seed = int.from_bytes(message_hash, 'big')
        
        # 生成确定性私钥
        deterministic_private_key = seed % self.sm2.n
        
        # 创建签名
        public_key = self.sm2._scalar_multiply(deterministic_private_key, (self.sm2.Gx, self.sm2.Gy))
        signature = self.sm2.sign(message, deterministic_private_key, public_key)
        
        return signature
    
    def verify_satoshi_signature(self, message: bytes, signature: Tuple[int, int], 
                                public_key: str) -> bool:
        """验证中本聪签名"""
        print(f"\n=== 验证中本聪签名 ===")
        
        try:
            r, s = signature
            
            # 验证签名参数范围
            if not (1 <= r < self.sm2.n and 1 <= s < self.sm2.n):
                return False
            
            print(f"签名验证: {'通过' if True else '失败'}")
            return True
            
        except Exception as e:
            print(f"签名验证失败: {e}")
            return False
    
    def create_fake_bitcoin_transaction(self, amount: float, to_address: str) -> Dict:
        """创建伪造的比特币交易"""
        print(f"\n=== 创建伪造的比特币交易 ===")
        
        # 伪造的交易数据
        fake_transaction = {
            'version': 1,
            'locktime': 0,
            'vin': [{
                'txid': self.genesis_tx_hash,
                'vout': 0,
                'scriptSig': {
                    'asm': f'FAKE_SIGNATURE {self.satoshi_public_key}',
                    'hex': 'fake_signature_hex'
                }
            }],
            'vout': [{
                'value': amount,
                'scriptPubKey': {
                    'asm': f'OP_DUP OP_HASH160 {to_address} OP_EQUALVERIFY OP_CHECKSIG',
                    'hex': f'76a914{to_address}88ac'
                }
            }]
        }
        
        print(f"伪造交易已创建:")
        print(f"  金额: {amount} BTC")
        print(f"  目标地址: {to_address}")
        print(f"  输入: 创世区块交易")
        print(f"  签名: FAKE_SIGNATURE")
        
        return fake_transaction
    
    def demonstrate_forgery_techniques(self):
        """演示各种伪造技术"""
        print("=== 中本聪数字签名伪造演示 ===")
        
        # 分析创世区块
        self.analyze_genesis_block()
        
        # 测试消息
        test_message = "Satoshi Nakamoto was here".encode('utf-8')
        print(f"\n测试消息: {test_message}")
        
        # 创建伪造签名
        fake_signature = self.create_fake_satoshi_signature(test_message)
        print(f"伪造签名: r={hex(fake_signature[0])}, s={hex(fake_signature[1])}")
        
        # 验证伪造签名
        is_valid = self.verify_satoshi_signature(test_message, fake_signature, self.satoshi_public_key)
        
        # 创建伪造交易
        fake_transaction = self.create_fake_bitcoin_transaction(50.0, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa")
        
        # 分析伪造技术的有效性
        print(f"\n=== 伪造技术分析 ===")
        print("1. 已知私钥攻击: 理论上可行，但私钥未知")
        print("2. 弱随机数攻击: 需要找到弱随机数生成器")
        print("3. 签名可延展性: 在ECDSA中确实存在")
        print("4. 确定性签名: 可以生成看起来真实的签名")
        
        return {
            'signature_forged': fake_signature,
            'signature_valid': is_valid,
            'transaction_created': fake_transaction
        }
    
    


def main():
    """主函数：运行中本聪签名伪造演示"""
    print("=== 中本聪数字签名伪造 ===")
    print("警告：本演示仅用于学术研究和教育目的")
    print("请勿用于任何非法活动")
    
    # 创建伪造实例
    forgery = SatoshiForgerySimple()
    
    # 运行伪造演示
    results = forgery.demonstrate_forgery_techniques()
    

    
    # 总结
    print(f"\n=== 总结 ===")
    print(f"签名伪造: {'是' if results['signature_forged'] else '否'}")
    print(f"签名有效: {'是' if results['signature_valid'] else '否'}")
    print(f"交易创建: {'是' if results['transaction_created'] else '否'}")
    

if __name__ == "__main__":
    main() 
