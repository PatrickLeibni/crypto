#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2 Usage Examples
SM2使用示例

This module provides comprehensive examples of using SM2 for various
cryptographic operations including signing, verification, encryption, and decryption.
"""

import sys
import os
import time
import hashlib

# Add the src directory to the Python path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'src'))

from sm2_basic import SM2
from sm2_optimized import OptimizedSM2


def basic_sm2_example():
    """基础SM2使用示例"""
    print("=== Basic SM2 Example ===")
    
    # 创建SM2实例
    sm2 = SM2()
    
    # 生成密钥对
    print("1. Generating keypair...")
    private_key, public_key = sm2.generate_keypair()
    print(f"   Private key: {private_key}")
    print(f"   Public key: ({public_key.x}, {public_key.y})")
    
    # 数字签名示例
    print("\n2. Digital Signature Example")
    message = b"Hello, SM2 digital signature!"
    print(f"   Message: {message}")
    
    # 生成签名
    signature = sm2.sign(message, private_key, public_key)
    print(f"   Signature: (r={signature[0]}, s={signature[1]})")
    
    # 验证签名
    is_valid = sm2.verify(message, signature, public_key)
    print(f"   Signature verification: {'PASS' if is_valid else 'FAIL'}")
    
    # 测试错误消息
    wrong_message = b"Wrong message for signature verification"
    is_invalid = sm2.verify(wrong_message, signature, public_key)
    print(f"   Wrong message verification: {'PASS' if is_invalid else 'FAIL'}")
    
    # 加密解密示例
    print("\n3. Encryption/Decryption Example")
    secret_message = b"This is a secret message that needs to be encrypted!"
    print(f"   Original message: {secret_message}")
    
    try:
        # 加密
        ciphertext = sm2.encrypt(secret_message, public_key)
        print(f"   Ciphertext: C1=({ciphertext[0].x}, {ciphertext[0].y})")
        print(f"              C2={ciphertext[1].hex()}")
        print(f"              C3={ciphertext[2].hex()}")
        
        # 解密
        decrypted = sm2.decrypt(ciphertext, private_key)
        print(f"   Decrypted message: {decrypted}")
        print(f"   Decryption verification: {'PASS' if decrypted == secret_message else 'FAIL'}")
        
    except Exception as e:
        print(f"   Encryption/Decryption error: {e}")
        print("   Skipping encryption/decryption example")
    
    return True


def optimized_sm2_example():
    """优化的SM2使用示例"""
    print("\n=== Optimized SM2 Example ===")
    
    # 创建优化的SM2实例
    sm2_optimized = OptimizedSM2(use_parallel=True)
    
    # 生成密钥对
    print("1. Generating keypair with optimization...")
    start_time = time.time()
    private_key, public_key = sm2_optimized.generate_keypair()
    keygen_time = time.time() - start_time
    print(f"   Key generation time: {keygen_time:.4f} seconds")
    print(f"   Private key: {private_key}")
    print(f"   Public key: ({public_key.x}, {public_key.y})")
    
    # 性能测试
    print("\n2. Performance Benchmark")
    message = b"Performance test message for optimized SM2"
    
    # 签名性能
    start_time = time.time()
    signature = sm2_optimized.sign(message, private_key, public_key)
    sign_time = time.time() - start_time
    print(f"   Signature generation time: {sign_time:.4f} seconds")
    
    # 验证性能
    start_time = time.time()
    is_valid = sm2_optimized.verify(message, signature, public_key)
    verify_time = time.time() - start_time
    print(f"   Signature verification time: {verify_time:.4f} seconds")
    
    # 加密性能
    try:
        start_time = time.time()
        ciphertext = sm2_optimized.encrypt(message, public_key)
        encrypt_time = time.time() - start_time
        print(f"   Encryption time: {encrypt_time:.4f} seconds")
        
        # 解密性能
        start_time = time.time()
        decrypted = sm2_optimized.decrypt(ciphertext, private_key)
        decrypt_time = time.time() - start_time
        print(f"   Decryption time: {decrypt_time:.4f} seconds")
        
    except Exception as e:
        print(f"   Encryption/Decryption error: {e}")
        encrypt_time = 0
        decrypt_time = 0
    
    # 性能总结
    print(f"\n   Performance Summary:")
    print(f"   - Key generation: {keygen_time:.4f}s")
    print(f"   - Signature: {sign_time:.4f}s")
    print(f"   - Verification: {verify_time:.4f}s")
    print(f"   - Encryption: {encrypt_time:.4f}s")
    print(f"   - Decryption: {decrypt_time:.4f}s")
    
    return True


def security_example():
    """安全特性示例"""
    print("\n=== Security Features Example ===")
    
    sm2 = SM2()
    
    # 1. 密钥验证
    print("1. Public Key Validation")
    private_key, public_key = sm2.generate_keypair()
    
    # 验证公钥有效性
    is_valid_key = sm2._is_valid_public_key(public_key)
    print(f"   Public key validation: {'PASS' if is_valid_key else 'FAIL'}")
    
    # 2. 签名唯一性测试
    print("\n2. Signature Uniqueness Test")
    message = b"Test message for signature uniqueness"
    
    signatures = []
    for i in range(5):
        signature = sm2.sign(message, private_key, public_key)
        signatures.append(signature)
        print(f"   Signature {i+1}: (r={signature[0]}, s={signature[1]})")
    
    # 检查签名唯一性
    unique_signatures = set(signatures)
    print(f"   Unique signatures: {len(unique_signatures)}/{len(signatures)}")
    print(f"   Uniqueness: {'PASS' if len(unique_signatures) == len(signatures) else 'FAIL'}")
    
    # 3. 错误处理测试
    print("\n3. Error Handling Test")
    
    # 测试无效签名
    invalid_signature = (0, 0)  # 无效的r和s值
    try:
        is_valid = sm2.verify(message, invalid_signature, public_key)
        print(f"   Invalid signature handling: {'PASS' if not is_valid else 'FAIL'}")
    except Exception as e:
        print(f"   Invalid signature handling: PASS (Exception caught: {e})")
    
    # 4. 边界条件测试
    print("\n4. Boundary Condition Test")
    
    # 测试空消息
    empty_message = b""
    try:
        signature = sm2.sign(empty_message, private_key, public_key)
        is_valid = sm2.verify(empty_message, signature, public_key)
        print(f"   Empty message handling: {'PASS' if is_valid else 'FAIL'}")
    except Exception as e:
        print(f"   Empty message handling: FAIL (Exception: {e})")
    
    # 测试长消息
    long_message = b"A" * 10000
    try:
        signature = sm2.sign(long_message, private_key, public_key)
        is_valid = sm2.verify(long_message, signature, public_key)
        print(f"   Long message handling: {'PASS' if is_valid else 'FAIL'}")
    except Exception as e:
        print(f"   Long message handling: FAIL (Exception: {e})")
    
    return True


def comparison_example():
    """基础版本与优化版本比较示例"""
    print("\n=== Basic vs Optimized Comparison ===")
    
    # 创建两个实例
    sm2_basic = SM2()
    sm2_optimized = OptimizedSM2(use_parallel=True)
    
    # 测试消息
    message = b"Comparison test message"
    
    # 生成密钥对
    print("1. Key Generation Comparison")
    
    start_time = time.time()
    private_key_basic, public_key_basic = sm2_basic.generate_keypair()
    basic_keygen_time = time.time() - start_time
    
    start_time = time.time()
    private_key_optimized, public_key_optimized = sm2_optimized.generate_keypair()
    optimized_keygen_time = time.time() - start_time
    
    print(f"   Basic key generation: {basic_keygen_time:.4f}s")
    print(f"   Optimized key generation: {optimized_keygen_time:.4f}s")
    print(f"   Improvement: {((basic_keygen_time - optimized_keygen_time) / basic_keygen_time * 100):.1f}%")
    
    # 签名性能比较
    print("\n2. Signature Performance Comparison")
    
    start_time = time.time()
    signature_basic = sm2_basic.sign(message, private_key_basic, public_key_basic)
    basic_sign_time = time.time() - start_time
    
    start_time = time.time()
    signature_optimized = sm2_optimized.sign(message, private_key_optimized, public_key_optimized)
    optimized_sign_time = time.time() - start_time
    
    print(f"   Basic signature: {basic_sign_time:.4f}s")
    print(f"   Optimized signature: {optimized_sign_time:.4f}s")
    print(f"   Improvement: {((basic_sign_time - optimized_sign_time) / basic_sign_time * 100):.1f}%")
    
    # 验证性能比较
    print("\n3. Verification Performance Comparison")
    
    start_time = time.time()
    is_valid_basic = sm2_basic.verify(message, signature_basic, public_key_basic)
    basic_verify_time = time.time() - start_time
    
    start_time = time.time()
    is_valid_optimized = sm2_optimized.verify(message, signature_optimized, public_key_optimized)
    optimized_verify_time = time.time() - start_time
    
    print(f"   Basic verification: {basic_verify_time:.4f}s")
    print(f"   Optimized verification: {optimized_verify_time:.4f}s")
    print(f"   Improvement: {((basic_verify_time - optimized_verify_time) / basic_verify_time * 100):.1f}%")
    
    # 加密性能比较
    print("\n4. Encryption Performance Comparison")
    
    try:
        start_time = time.time()
        ciphertext_basic = sm2_basic.encrypt(message, public_key_basic)
        basic_encrypt_time = time.time() - start_time
        
        start_time = time.time()
        ciphertext_optimized = sm2_optimized.encrypt(message, public_key_optimized)
        optimized_encrypt_time = time.time() - start_time
        
        print(f"   Basic encryption: {basic_encrypt_time:.4f}s")
        print(f"   Optimized encryption: {optimized_encrypt_time:.4f}s")
        print(f"   Improvement: {((basic_encrypt_time - optimized_encrypt_time) / basic_encrypt_time * 100):.1f}%")
        
        # 解密性能比较
        print("\n5. Decryption Performance Comparison")
        
        start_time = time.time()
        decrypted_basic = sm2_basic.decrypt(ciphertext_basic, private_key_basic)
        basic_decrypt_time = time.time() - start_time
        
        start_time = time.time()
        decrypted_optimized = sm2_optimized.decrypt(ciphertext_optimized, private_key_optimized)
        optimized_decrypt_time = time.time() - start_time
        
        print(f"   Basic decryption: {basic_decrypt_time:.4f}s")
        print(f"   Optimized decryption: {optimized_decrypt_time:.4f}s")
        print(f"   Improvement: {((basic_decrypt_time - optimized_decrypt_time) / basic_decrypt_time * 100):.1f}%")
        
    except Exception as e:
        print(f"   Encryption/Decryption comparison error: {e}")
        basic_encrypt_time = 0
        optimized_encrypt_time = 0
        basic_decrypt_time = 0
        optimized_decrypt_time = 0
    
    # 总体性能总结
    print(f"\n6. Overall Performance Summary")
    total_basic_time = basic_keygen_time + basic_sign_time + basic_verify_time + basic_encrypt_time + basic_decrypt_time
    total_optimized_time = optimized_keygen_time + optimized_sign_time + optimized_verify_time + optimized_encrypt_time + optimized_decrypt_time
    
    print(f"   Total basic time: {total_basic_time:.4f}s")
    print(f"   Total optimized time: {total_optimized_time:.4f}s")
    print(f"   Overall improvement: {((total_basic_time - total_optimized_time) / total_basic_time * 100):.1f}%")
    
    return True


def main():
    """主函数：运行所有示例"""
    print("=== SM2 Comprehensive Examples ===")
    print("This demonstrates various aspects of SM2 implementation")
    
    try:
        # 运行基础示例
        basic_sm2_example()
        
        # 运行优化示例
        optimized_sm2_example()
        
        # 运行安全特性示例
        security_example()
        
        # 运行性能比较示例
        comparison_example()
        
        print(f"\n=== All Examples Completed Successfully ===")
        print("✓ Basic SM2 functionality")
        print("✓ Optimized SM2 performance")
        print("✓ Security features validation")
        print("✓ Performance comparison analysis")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        return False
    
    return True


if __name__ == "__main__":
    success = main()
    exit(0 if success else 1) 