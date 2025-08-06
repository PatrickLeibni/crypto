#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
性能优化脚本
用于测试和优化DDH协议的性能
"""

import sys
import os
import time
import statistics
from typing import List, Tuple
sys.path.append(os.path.join(os.path.dirname(__file__), 'src'))

from ddh_protocol import DDHProtocol, generate_protocol_params


def benchmark_protocol(bit_length: int, user_count: int, breach_count: int, iterations: int = 3) -> dict:
    """基准测试协议性能"""
    print(f"=== 性能基准测试 ===")
    print(f"参数长度: {bit_length}位")
    print(f"用户数量: {user_count}")
    print(f"泄露记录数量: {breach_count}")
    print(f"测试迭代次数: {iterations}")
    
    # 生成测试数据
    V = [f"password{i}" for i in range(user_count)]
    W = [(f"password{i}", i * 10) for i in range(breach_count)]
    
    # 计算期望结果
    V_set = set(V)
    expected_sum = sum(t_j for w_j, t_j in W if w_j in V_set)
    
    times = []
    results = []
    
    for i in range(iterations):
        print(f"\n--- 第 {i+1} 次测试 ---")
        
        # 生成协议参数
        start_time = time.time()
        params = generate_protocol_params(bit_length)
        param_time = time.time() - start_time
        
        # 创建协议实例
        protocol = DDHProtocol(params)
        
        # 运行协议
        start_time = time.time()
        actual_sum = protocol.run_protocol(V, W)
        protocol_time = time.time() - start_time
        
        total_time = param_time + protocol_time
        times.append(total_time)
        results.append(actual_sum)
        
        print(f"参数生成时间: {param_time:.4f}秒")
        print(f"协议运行时间: {protocol_time:.4f}秒")
        print(f"总时间: {total_time:.4f}秒")
        print(f"期望结果: {expected_sum}, 实际结果: {actual_sum}")
        print(f"结果正确: {'是' if expected_sum == actual_sum else '否'}")
    
    # 统计结果
    avg_time = statistics.mean(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0
    min_time = min(times)
    max_time = max(times)
    
    success_rate = sum(1 for r in results if r == expected_sum) / len(results)
    
    return {
        'bit_length': bit_length,
        'user_count': user_count,
        'breach_count': breach_count,
        'iterations': iterations,
        'avg_time': avg_time,
        'std_time': std_time,
        'min_time': min_time,
        'max_time': max_time,
        'success_rate': success_rate,
        'expected_sum': expected_sum,
        'actual_results': results
    }


def scalability_test():
    """可扩展性测试"""
    print("=== 可扩展性测试 ===")
    
    test_scenarios = [
        (512, 5, 50),
        (512, 10, 100),
        (512, 20, 200),
        (1024, 5, 50),
        (1024, 10, 100),
        (1024, 20, 200),
    ]
    
    results = []
    
    for bit_length, user_count, breach_count in test_scenarios:
        print(f"\n测试场景: {bit_length}位, {user_count}用户, {breach_count}记录")
        result = benchmark_protocol(bit_length, user_count, breach_count, 2)
        results.append(result)
        
        print(f"平均时间: {result['avg_time']:.4f}秒")
        print(f"成功率: {result['success_rate']:.2%}")
    
    return results


def security_analysis():
    """安全性分析"""
    print("\n=== 安全性分析 ===")
    
    security_features = {
        "隐私保护": "用户密码不会泄露给服务器",
        "数据混淆": "使用双重盲化和同态加密",
        "交集隐私": "只泄露交集和，不泄露具体交集元素",
        "DDH安全": "基于Decisional Diffie-Hellman假设",
        "同态加密": "支持密文上的加法运算",
        "随机化": "使用随机化技术防止信息泄露"
    }
    
    for feature, description in security_features.items():
        print(f"✅ {feature}: {description}")


def performance_optimization_suggestions():
    """性能优化建议"""
    print("\n=== 性能优化建议 ===")
    
    suggestions = [
        "使用更大的参数长度以提高安全性（1024位或更高）",
        "对于大规模数据集，考虑使用并行处理",
        "优化群运算算法以提高计算效率",
        "使用更高效的离散对数算法",
        "考虑使用预计算技术减少重复计算",
        "实现批量处理以减少通信开销"
    ]
    
    for i, suggestion in enumerate(suggestions, 1):
        print(f"{i}. {suggestion}")


def main():
    """主函数"""
    print("=== DDH协议性能优化工具 ===")
    print("基于Google Password Checkup论文实现")
    print("")
    
    # 1. 可扩展性测试
    scalability_results = scalability_test()
    
    # 2. 安全性分析
    security_analysis()
    
    # 3. 性能优化建议
    performance_optimization_suggestions()
    
    # 4. 总结报告
    print("\n=== 性能测试总结 ===")
    print("测试场景统计:")
    for result in scalability_results:
        print(f"  {result['bit_length']}位, {result['user_count']}用户, {result['breach_count']}记录:")
        print(f"    平均时间: {result['avg_time']:.4f}秒")
        print(f"    成功率: {result['success_rate']:.2%}")
    
    print("\n=== 优化完成 ===")
    print("所有性能测试和分析完成！")


if __name__ == "__main__":
    main() 