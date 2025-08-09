#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同态加密实现
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'src'))

from ddh_protocol import HomomorphicEncryption, generate_protocol_params


def test_homomorphic_encryption():
    """测试同态加密"""
    print("=== 测试同态加密 ===")
    
    # 生成参数
    params = generate_protocol_params(256)  # 使用较小的参数进行测试
    he = HomomorphicEncryption(params)
    
    # 生成密钥对
    pk, sk = he.key_gen()
    print(f"公钥: {pk}")
    print(f"私钥: {sk}")
    
    # 测试基本加密解密
    test_messages = [1, 5, 10, 100]
    for msg in test_messages:
        ciphertext = he.encrypt(msg)
        decrypted = he.decrypt(ciphertext)
        print(f"消息: {msg}, 密文: {ciphertext}, 解密: {decrypted}, 正确: {msg == decrypted}")
    
    # 测试同态加法
    m1, m2 = 10, 20
    c1 = he.encrypt(m1)
    c2 = he.encrypt(m2)
    
    print(f"\n同态加法测试:")
    print(f"消息1: {m1}, 密文1: {c1}")
    print(f"消息2: {m2}, 密文2: {c2}")
    
    # 同态加法
    c_sum = he.add(c1, c2)
    print(f"同态加法结果: {c_sum}")
    
    # 解密
    result = he.decrypt(c_sum)
    print(f"解密结果: {result}")
    print(f"期望结果: {m1 + m2}")
    print(f"验证: {result == m1 + m2}")
    
    return result == m1 + m2


def test_multiple_additions():
    """测试多个数的同态加法"""
    print("\n=== 测试多个数的同态加法 ===")
    
    params = generate_protocol_params(256)
    he = HomomorphicEncryption(params)
    pk, sk = he.key_gen()
    
    messages = [1, 2, 3, 4, 5]
    expected_sum = sum(messages)
    
    print(f"消息列表: {messages}")
    print(f"期望和: {expected_sum}")
    
    # 加密所有消息
    ciphertexts = [he.encrypt(msg) for msg in messages]
    
    # 同态求和
    sum_ciphertext = ciphertexts[0]
    for ciphertext in ciphertexts[1:]:
        sum_ciphertext = he.add(sum_ciphertext, ciphertext)
    
    # 解密
    actual_sum = he.decrypt(sum_ciphertext)
    print(f"实际和: {actual_sum}")
    print(f"验证: {actual_sum == expected_sum}")
    
    return actual_sum == expected_sum


if __name__ == "__main__":
    print("开始同态加密测试...")
    
    # 测试1: 基本加密解密
    test1_result = test_homomorphic_encryption()
    
    # 测试2: 多个数的同态加法
    test2_result = test_multiple_additions()
    
    print(f"\n=== 测试结果 ===")
    print(f"基本加密解密测试: {'通过' if test1_result else '失败'}")
    print(f"同态加法测试: {'通过' if test2_result else '失败'}")
    
    if test1_result and test2_result:
        print("✅ 所有测试通过！")
    else:
        print("❌ 部分测试失败！") 