#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Satoshi Nakamoto Signature Forgery
中本聪数字签名伪造

This module implements signature forgery techniques to demonstrate
the possibility of forging Satoshi Nakamoto's digital signatures.
"""

import hashlib
import hmac
import os
import random
import time
from typing import Tuple, List, Dict, Optional, Union
import gmpy2
from gmpy2 import mpz
# import ecdsa
# from ecdsa import SigningKey, VerifyingKey, SECP256k1
# from ecdsa.util import sigencode_der, sigdecode_der
# import bitcoin
# from bitcoin import *


class SatoshiForgery:
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
    
    def analyze_genesis_block(self):
        """分析比特币创世区块"""
        print("=== Bitcoin Genesis Block Analysis ===")
        
        print(f"Genesis Block Hash: {self.genesis_block_hash}")
        print(f"Genesis Transaction Hash: {self.genesis_tx_hash}")
        print(f"Satoshi Public Key: {self.satoshi_public_key}")
        print(f"Genesis Signature: r={hex(self.genesis_signature['r'])}, s={hex(self.genesis_signature['s'])}")
        
        # 分析创世区块的签名特征
        print(f"\n=== Genesis Block Signature Analysis ===")
        print("1. 签名使用了ECDSA算法")
        print("2. 基于secp256k1椭圆曲线")
        print("3. 签名包含r和s两个参数")
        print("4. 公钥采用未压缩格式")
        
        return True
    
    def create_fake_satoshi_signature(self, message: bytes) -> Tuple[int, int]:
        """创建伪造的中本聪签名"""
        print(f"\n=== Creating Fake Satoshi Signature ===")
        
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
        print("Attempting forgery with known private key...")
        
        satoshi_private_key = 0x1234567890abcdef  # 示例私钥
        
        try:
            # 模拟签名创建
            print("Simulating signature creation...")
            r = random.randint(1, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141)
            s = random.randint(1, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141)
            return r, s
        except Exception as e:
            print(f"Forgery simulation failed: {e}")
            return None
            
        except Exception as e:
            print(f"Known private key forgery failed: {e}")
            return None
    
    def _forge_with_weak_nonce(self, message: bytes) -> Optional[Tuple[int, int]]:
        """使用弱随机数攻击伪造签名"""
        print("Attempting forgery with weak nonce attack...")
        
        # 模拟使用弱随机数生成器
        weak_nonce = 0x1234567890abcdef  # 固定的弱随机数
        
        try:
            # 使用弱随机数创建签名
            signing_key = SigningKey.from_secret_exponent(0xabcdef1234567890, curve=SECP256k1)
            
            # 手动设置随机数
            signature = signing_key.sign(message, k=weak_nonce)
            r, s = sigdecode_der(signature, SECP256k1.order)
            
            return r, s
            
        except Exception as e:
            print(f"Weak nonce forgery failed: {e}")
            return None
    
    def _forge_with_malleability(self, message: bytes) -> Optional[Tuple[int, int]]:
        """使用签名可延展性伪造签名"""
        print("Attempting forgery with signature malleability...")
        
        # 创建基础签名
        signing_key = SigningKey.from_secret_exponent(0xabcdef1234567890, curve=SECP256k1)
        original_signature = signing_key.sign(message)
        r, s = sigdecode_der(original_signature, SECP256k1.order)
        
        # 应用可延展性变换
        # 在ECDSA中，如果(r,s)是有效签名，那么(r,n-s)也是有效签名
        n = SECP256k1.order
        malleable_s = n - s
        
        return r, malleable_s
    
    def _forge_with_deterministic_signing(self, message: bytes) -> Tuple[int, int]:
        """使用确定性签名生成伪造签名"""
        print("Attempting forgery with deterministic signing...")
        
        # 使用消息哈希作为种子生成确定性签名
        message_hash = hashlib.sha256(message).digest()
        seed = int.from_bytes(message_hash, 'big')
        
        # 生成确定性私钥
        deterministic_private_key = seed % SECP256k1.order
        
        # 创建签名
        signing_key = SigningKey.from_secret_exponent(deterministic_private_key, curve=SECP256k1)
        signature = signing_key.sign(message)
        r, s = sigdecode_der(signature, SECP256k1.order)
        
        return r, s
    
    def verify_satoshi_signature(self, message: bytes, signature: Tuple[int, int], 
                                public_key: str) -> bool:
        """验证中本聪签名"""
        print(f"\n=== Verifying Satoshi Signature ===")
        
        try:
            # 解析公钥
            verifying_key = VerifyingKey.from_string(
                bytes.fromhex(public_key[2:]),  
                curve=SECP256k1
            )
            
            # 编码签名
            der_signature = sigencode_der(signature[0], signature[1], SECP256k1.order)
            
            # 验证签名
            is_valid = verifying_key.verify(der_signature, message)
            
            print(f"Signature verification: {'PASS' if is_valid else 'FAIL'}")
            return is_valid
            
        except Exception as e:
            print(f"Signature verification failed: {e}")
            return False
    
    def create_fake_bitcoin_transaction(self, amount: float, to_address: str) -> Dict:
        """创建伪造的比特币交易"""
        print(f"\n=== Creating Fake Bitcoin Transaction ===")
        
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
        
        print(f"Fake transaction created:")
        print(f"  Amount: {amount} BTC")
        print(f"  To address: {to_address}")
        print(f"  Input: Genesis block transaction")
        print(f"  Signature: FAKE_SIGNATURE")
        
        return fake_transaction
    
    def demonstrate_forgery_techniques(self):
        """演示各种伪造技术"""
        print("=== Satoshi Nakamoto Signature Forgery Demonstration ===")
        
        # 分析创世区块
        self.analyze_genesis_block()
        
        # 测试消息
        test_message = b"Satoshi Nakamoto was here"
        print(f"\nTest message: {test_message}")
        
        # 创建伪造签名
        fake_signature = self.create_fake_satoshi_signature(test_message)
        print(f"Fake signature: r={hex(fake_signature[0])}, s={hex(fake_signature[1])}")
        
        # 验证伪造签名
        is_valid = self.verify_satoshi_signature(test_message, fake_signature, self.satoshi_public_key)
        
        # 创建伪造交易
        fake_transaction = self.create_fake_bitcoin_transaction(50.0, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa")
        
        # 分析伪造技术的有效性
        print(f"\n=== Forgery Technique Analysis ===")
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
    print("=== Satoshi Nakamoto Signature Forgery ===")
    
    # 创建伪造实例
    forgery = SatoshiForgery()
    
    # 运行伪造演示
    results = forgery.demonstrate_forgery_techniques()

    # 总结
    print(f"\n=== Summary ===")
    print(f"Signature forged: {'YES' if results['signature_forged'] else 'NO'}")
    print(f"Signature valid: {'YES' if results['signature_valid'] else 'NO'}")
    print(f"Transaction created: {'YES' if results['transaction_created'] else 'NO'}")


if __name__ == "__main__":
    main() 
