#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Comprehensive SM2 Project 5 Demo
SM2项目5综合演示

This script demonstrates all three requirements:
a) SM2 basic implementation and optimization in Python
b) Signature algorithm misuse POC based on 20250713-wen-sm2-public.pdf
c) Satoshi Nakamoto signature forgery
"""

import time
import sys
from sm2_simple import SimpleSM2
from sm2_misuse_poc import SM2MisusePOC
from satoshi_forgery_simple import SatoshiForgerySimple


def demo_requirement_a():
    """演示要求a: SM2基础实现和优化"""
    print("=" * 60)
    print("要求a: SM2基础实现和优化")
    print("=" * 60)
     
    # 创建SM2实例
    sm2 = SimpleSM2()
    
    # 性能测试
    print("\n1. 性能基准测试")
    start_time = time.time()
    
    # 生成密钥对
    private_key, public_key = sm2.generate_keypair()
    keygen_time = time.time() - start_time
    
    # 签名测试
    message = "Hello, SM2!".encode('utf-8')
    start_time = time.time()
    signature = sm2.sign(message, private_key, public_key)
    sign_time = time.time() - start_time
    
    # 验证测试
    start_time = time.time()
    is_valid = sm2.verify(message, signature, public_key)
    verify_time = time.time() - start_time
    
    # 加密测试
    secret_message = "Secret message for encryption".encode('utf-8')
    start_time = time.time()
    ciphertext = sm2.encrypt(secret_message, public_key)
    encrypt_time = time.time() - start_time
    
    # 解密测试
    start_time = time.time()
    decrypted = sm2.decrypt(ciphertext, private_key)
    decrypt_time = time.time() - start_time
    
    print(f"密钥生成时间: {keygen_time:.4f}秒")
    print(f"签名生成时间: {sign_time:.4f}秒")
    print(f"签名验证时间: {verify_time:.4f}秒")
    print(f"加密时间: {encrypt_time:.4f}秒")
    print(f"解密时间: {decrypt_time:.4f}秒")
    print(f"总时间: {keygen_time + sign_time + verify_time + encrypt_time + decrypt_time:.4f}秒")
    
    print(f"\n验证结果:")
    print(f"签名验证: {'通过' if is_valid else '失败'}")
    print(f"解密验证: {'通过' if decrypted == secret_message else '失败'}")
    
    print(f"\n✅ 要求a完成: SM2基础实现和优化")
    return True


def demo_requirement_b():
    """演示要求b: 签名算法误用POC验证"""
    print("\n" + "=" * 60)
    print("要求b: 签名算法误用POC验证")
    print("=" * 60)
    
    print("基于20250713-wen-sm2-public.pdf中提到的签名算法误用进行POC验证")
    
    # 创建POC实例
    misuse_poc = SM2MisusePOC()
    
    # 运行综合攻击演示
    results = misuse_poc.comprehensive_attack_demo()
    
    print(f"\n攻击成功率统计:")
    success_count = sum(results.values())
    total_count = len(results)
    print(f"成功攻击: {success_count}/{total_count}")
    print(f"成功率: {success_count/total_count*100:.1f}%")
    
    print(f"\n✅ 要求b完成: 签名算法误用POC验证")
    return True


def demo_requirement_c():
    """演示要求c: 中本聪数字签名伪造"""
    print("\n" + "=" * 60)
    print("要求c: 中本聪数字签名伪造")
    print("=" * 60)
    
    print("伪造中本聪的数字签名演示")
    
    # 创建伪造实例
    forgery = SatoshiForgerySimple()
    
    # 运行伪造演示
    results = forgery.demonstrate_forgery_techniques()
    
    print(f"\n伪造结果:")
    print(f"签名伪造: {'是' if results['signature_forged'] else '否'}")
    print(f"签名有效: {'是' if results['signature_valid'] else '否'}")
    print(f"交易创建: {'是' if results['transaction_created'] else '否'}")
    
    print(f"\n✅ 要求c完成: 中本聪数字签名伪造")
    return True

def main():
    """主函数：运行综合演示"""
    print("SM2项目5综合演示")
    print("=" * 60)
    print("本演示将展示所有三个要求的完成情况")
    print("=" * 60)
    
    try:
        # 演示要求a
        demo_requirement_a()
        
        # 演示要求b
        demo_requirement_b()
        
        # 演示要求c
        demo_requirement_c()
        
        
        # 最终总结
        print("\n" + "=" * 60)
        print("项目5完成总结")
        print("=" * 60)
        
        print("✅ 要求a: SM2基础实现和优化")
        print("   - 使用Python实现SM2椭圆曲线密码算法")
        print("   - 包含密钥生成、签名、验证、加密、解密功能")
        print("   - 性能优化和基准测试")
        
        print("\n✅ 要求b: 签名算法误用POC验证")
        print("   - 基于20250713-wen-sm2-public.pdf的分析")
        print("   - 重放攻击、随机数重用、签名可延展性、密钥恢复攻击")
        print("   - 完整的推导文档和验证代码")
        
        print("\n✅ 要求c: 中本聪数字签名伪造")
        print("   - 比特币创世区块分析")
        print("   - 签名伪造技术实现")
        print("   - 交易伪造演示和安全影响分析")
        
        print("\n" + "=" * 60)
        print("所有要求已完成！")
        print("=" * 60)
        
        return True
        
    except Exception as e:
        print(f"演示过程中出现错误: {e}")
        return False


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1) 
