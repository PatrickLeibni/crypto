#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Basic Demo for Google Password Checkup
Google密码检查基础演示
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'src'))

from ddh_protocol import DDHProtocol, generate_protocol_params
from password_checkup import GooglePasswordCheckup, simulate_real_world_scenario


def demo_protocol_basics():
    """演示协议基础功能"""
    print("=== DDH协议基础演示 ===")
    
    # 生成协议参数
    params = generate_protocol_params(512)
    protocol = DDHProtocol(params)
    
    # 准备测试数据
    V = ["password1", "password2", "password3"]
    W = [("password1", 10), ("password2", 20), ("password4", 30)]
    
    print(f"P1的标识符集合: {V}")
    print(f"P2的标识符-值对: {W}")
    
    # 计算期望的交集和
    V_set = set(V)
    expected_sum = sum(t_j for w_j, t_j in W if w_j in V_set)
    print(f"期望的交集和: {expected_sum}")
    
    # 运行协议
    actual_sum = protocol.run_protocol(V, W)
    
    # 验证结果
    print(f"\n=== 结果验证 ===")
    print(f"期望的交集和: {expected_sum}")
    print(f"协议计算的结果: {actual_sum}")
    print(f"结果正确: {'是' if expected_sum == actual_sum else '否'}")
    
    return expected_sum == actual_sum


def demo_homomorphic_encryption():
    """演示同态加密"""
    print("\n=== 同态加密演示 ===")
    
    from ddh_protocol import HomomorphicEncryption
    
    # 生成参数
    params = generate_protocol_params(512)
    he = HomomorphicEncryption(params)
    
    # 生成密钥对
    pk, sk = he.key_gen()
    print(f"公钥: {pk}")
    print(f"私钥: {sk}")
    
    # 加密测试
    m1, m2 = 10, 20
    c1 = he.encrypt(m1)
    c2 = he.encrypt(m2)
    
    print(f"消息1: {m1}, 密文1: {c1}")
    print(f"消息2: {m2}, 密文2: {c2}")
    
    # 同态加法
    c_sum = he.add(c1, c2)
    print(f"同态加法结果: {c_sum}")
    
    # 解密
    result = he.decrypt(c_sum)
    print(f"解密结果: {result}")
    print(f"验证: {result == m1 + m2}")
    
    return result == m1 + m2


def demo_password_checkup():
    """演示密码检查服务"""
    print("\n=== Google Password Checkup 演示 ===")
    
    # 运行真实场景模拟
    results = simulate_real_world_scenario()
    
    return results


def demo_performance_comparison():
    """演示性能比较"""
    print("\n=== 性能比较演示 ===")
    
    # 不同规模的测试
    test_scenarios = [
        (5, 50),
        (10, 100),
        (20, 200)
    ]
    
    for user_count, breach_count in test_scenarios:
        print(f"\n测试规模: {user_count} 用户, {breach_count} 泄露记录")
        
        # 生成测试数据
        V = [f"password{i}" for i in range(user_count)]
        W = [(f"password{i}", i * 10) for i in range(breach_count)]
        
        # 创建协议
        params = generate_protocol_params(1024)
        protocol = DDHProtocol(params)
        
        # 测量性能
        import time
        start_time = time.time()
        result = protocol.run_protocol(V, W)
        end_time = time.time()
        
        print(f"执行时间: {end_time - start_time:.4f}秒")
        print(f"结果: {result}")


def demo_security_features():
    """演示安全特性"""
    print("\n=== 安全特性演示 ===")
    
    security_features = {
        "隐私保护": "用户密码不会泄露给Google",
        "数据混淆": "使用双重盲化和同态加密",
        "交集隐私": "只泄露交集和，不泄露具体交集元素",
        "DDH安全": "基于Decisional Diffie-Hellman假设",
        "同态加密": "支持密文上的加法运算"
    }
    
    for feature, description in security_features.items():
        print(f"✅ {feature}: {description}")


def main():
    """主函数"""
    print("=== Google Password Checkup 基础演示 ===")
    print("基于论文: https://eprint.iacr.org/2019/723.pdf")
    print("Section 3.1, Figure 2: Protocol ΠDDH")
    print("")
    
    # 运行各个演示
    results = []
    
    # 1. 协议基础演示
    results.append(demo_protocol_basics())
    
    # 2. 同态加密演示
    results.append(demo_homomorphic_encryption())
    
    # 3. 密码检查演示
    demo_password_checkup()
    
    # 4. 性能比较演示
    demo_performance_comparison()
    
    # 5. 安全特性演示
    demo_security_features()
    
    # 总结
    print("\n=== 演示总结 ===")
    print(f"协议基础测试: {'通过' if results[0] else '失败'}")
    print(f"同态加密测试: {'通过' if results[1] else '失败'}")
    print("✅ 密码检查服务演示")
    print("✅ 性能比较演示")
    print("✅ 安全特性演示")
    
    print("\n=== 演示完成 ===")
    print("所有功能演示完成！")


if __name__ == "__main__":
    main() 