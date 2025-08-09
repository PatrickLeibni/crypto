#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SM2 Performance Comparison
SM2性能比较测试

该脚本用于比较SM2基础实现和优化实现的性能差异。
"""

import time
from sm2_basic import SM2
from sm2_optimized import OptimizedSM2


def benchmark_sm2_implementation(implementation, name, iterations=1000, use_parallel=None):
    """测试特定SM2实现的性能"""
    print(f"=== {name}性能基准测试 ({iterations} 次迭代) ===")
    
    # 创建实例
    if use_parallel is not None:
        sm2 = implementation(use_parallel=use_parallel)
    else:
        sm2 = implementation()
    
    # 生成密钥对
    print("生成密钥对...")
    start_time = time.time()
    private_key, public_key = sm2.generate_keypair()
    keygen_time = time.time() - start_time
    print(f"密钥生成时间: {keygen_time:.4f} 秒")
    
    # 准备测试消息
    message = b"Performance test message for SM2 implementation comparison"
    
    # 测试签名性能
    print(f"\n测试签名性能...")
    start_time = time.time()
    for _ in range(iterations):
        signature = sm2.sign(message, private_key, public_key)
    sign_time = time.time() - start_time
    avg_sign_time = sign_time / iterations
    print(f"平均签名时间: {avg_sign_time:.6f} 秒")
    
    # 测试验证性能
    print(f"测试验证性能...")
    start_time = time.time()
    for _ in range(iterations):
        is_valid = sm2.verify(message, signature, public_key)
    verify_time = time.time() - start_time
    avg_verify_time = verify_time / iterations
    print(f"平均验证时间: {avg_verify_time:.6f} 秒")
    
    # 测试加密性能
    print(f"\n测试加密性能...")
    start_time = time.time()
    for _ in range(iterations):
        ciphertext = sm2.encrypt(message, public_key)
    encrypt_time = time.time() - start_time
    avg_encrypt_time = encrypt_time / iterations
    print(f"平均加密时间: {avg_encrypt_time:.6f} 秒")
    
    # 测试解密性能
    print(f"测试解密性能...")
    start_time = time.time()
    for _ in range(iterations):
        decrypted = sm2.decrypt(ciphertext, private_key)
    decrypt_time = time.time() - start_time
    avg_decrypt_time = decrypt_time / iterations
    print(f"平均解密时间: {avg_decrypt_time:.6f} 秒")
    
    # 性能总结
    total_time = keygen_time + sign_time + verify_time + encrypt_time + decrypt_time
    print(f"\n=== 性能总结 ===")
    print(f"密钥生成: {keygen_time:.4f}秒")
    print(f"签名: {avg_sign_time:.6f}秒/次")
    print(f"验证: {avg_verify_time:.6f}秒/次")
    print(f"加密: {avg_encrypt_time:.6f}秒/次")
    print(f"解密: {avg_decrypt_time:.6f}秒/次")
    print(f"总操作次数: {iterations * 4}")
    print(f"总时间: {total_time:.4f}秒")
    
    return {
        'name': name,
        'keygen_time': keygen_time,
        'avg_sign_time': avg_sign_time,
        'avg_verify_time': avg_verify_time,
        'avg_encrypt_time': avg_encrypt_time,
        'avg_decrypt_time': avg_decrypt_time,
        'total_time': total_time
    }


def compare_performance(iterations=1000, test_parallel_options=False):
    """比较SM2基础实现和优化实现的性能"""
    print("=" * 80)
    print("SM2实现性能比较测试")
    print("=" * 80)
    
    # 测试基础实现
    basic_results = benchmark_sm2_implementation(SM2, "SM2基础实现", iterations)
    
    print("\n" + "=" * 80)
    
    if test_parallel_options:
        # 测试不同并行选项
        print("测试优化实现(禁用并行)...")
        optimized_no_parallel = benchmark_sm2_implementation(
            lambda use_parallel: OptimizedSM2(use_parallel=use_parallel), 
            "SM2优化实现(禁用并行)", 
            iterations, 
            use_parallel=False
        )
        
        print("\n" + "=" * 80)
        
        print("测试优化实现(启用并行)...")
        optimized_with_parallel = benchmark_sm2_implementation(
            lambda use_parallel: OptimizedSM2(use_parallel=use_parallel), 
            "SM2优化实现(启用并行)", 
            iterations, 
            use_parallel=True
        )
        
        # 比较结果
        print("\n" + "=" * 80)
        print("SM2性能比较结果")
        print("=" * 80)
        
        print(f"{'指标':<15} {'基础实现':<15} {'优化(禁用并行)':<15} {'优化(启用并行)':<15} {'禁用并行提升':<15} {'启用并行提升':<15}")
        print("-" * 100)
        
        # 计算并打印各项指标
        # 密钥生成
        keygen_speedup_no_parallel = basic_results['keygen_time'] / optimized_no_parallel['keygen_time']
        keygen_speedup_with_parallel = basic_results['keygen_time'] / optimized_with_parallel['keygen_time']
        print(f"{'密钥生成时间':<15} {basic_results['keygen_time']:.4f}s    {optimized_no_parallel['keygen_time']:.4f}s    {optimized_with_parallel['keygen_time']:.4f}s    {keygen_speedup_no_parallel:.2f}x          {keygen_speedup_with_parallel:.2f}x")
        
        # 签名
        sign_speedup_no_parallel = basic_results['avg_sign_time'] / optimized_no_parallel['avg_sign_time']
        sign_speedup_with_parallel = basic_results['avg_sign_time'] / optimized_with_parallel['avg_sign_time']
        print(f"{'平均签名时间':<15} {basic_results['avg_sign_time']:.6f}s  {optimized_no_parallel['avg_sign_time']:.6f}s  {optimized_with_parallel['avg_sign_time']:.6f}s  {sign_speedup_no_parallel:.2f}x          {sign_speedup_with_parallel:.2f}x")
        
        # 验证
        verify_speedup_no_parallel = basic_results['avg_verify_time'] / optimized_no_parallel['avg_verify_time']
        verify_speedup_with_parallel = basic_results['avg_verify_time'] / optimized_with_parallel['avg_verify_time']
        print(f"{'平均验证时间':<15} {basic_results['avg_verify_time']:.6f}s  {optimized_no_parallel['avg_verify_time']:.6f}s  {optimized_with_parallel['avg_verify_time']:.6f}s  {verify_speedup_no_parallel:.2f}x          {verify_speedup_with_parallel:.2f}x")
        
        # 加密
        encrypt_speedup_no_parallel = basic_results['avg_encrypt_time'] / optimized_no_parallel['avg_encrypt_time']
        encrypt_speedup_with_parallel = basic_results['avg_encrypt_time'] / optimized_with_parallel['avg_encrypt_time']
        print(f"{'平均加密时间':<15} {basic_results['avg_encrypt_time']:.6f}s  {optimized_no_parallel['avg_encrypt_time']:.6f}s  {optimized_with_parallel['avg_encrypt_time']:.6f}s  {encrypt_speedup_no_parallel:.2f}x          {encrypt_speedup_with_parallel:.2f}x")
        
        # 解密
        decrypt_speedup_no_parallel = basic_results['avg_decrypt_time'] / optimized_no_parallel['avg_decrypt_time']
        decrypt_speedup_with_parallel = basic_results['avg_decrypt_time'] / optimized_with_parallel['avg_decrypt_time']
        print(f"{'平均解密时间':<15} {basic_results['avg_decrypt_time']:.6f}s  {optimized_no_parallel['avg_decrypt_time']:.6f}s  {optimized_with_parallel['avg_decrypt_time']:.6f}s  {decrypt_speedup_no_parallel:.2f}x          {decrypt_speedup_with_parallel:.2f}x")
        
        # 总时间
        total_speedup_no_parallel = basic_results['total_time'] / optimized_no_parallel['total_time']
        total_speedup_with_parallel = basic_results['total_time'] / optimized_with_parallel['total_time']
        print(f"{'总时间':<15} {basic_results['total_time']:.4f}s    {optimized_no_parallel['total_time']:.4f}s    {optimized_with_parallel['total_time']:.4f}s    {total_speedup_no_parallel:.2f}x          {total_speedup_with_parallel:.2f}x")
        
        return  # 添加return语句，避免执行后续代码
    else:
        # 测试优化实现(默认配置)
        optimized_results = benchmark_sm2_implementation(
            lambda use_parallel: OptimizedSM2(use_parallel=use_parallel), 
            "SM2优化实现", 
            iterations, 
            use_parallel=True
        )
        
        # 比较结果(保持原有代码)...
    
    # 比较结果
    print("\n" + "=" * 80)
    print("SM2性能比较结果")
    print("=" * 80)
    
    print(f"{'指标':<15} {'基础实现':<15} {'优化(禁用并行)':<15} {'优化(启用并行)':<15} {'禁用并行提升':<15} {'启用并行提升':<15}")
    print("-" * 100)
    
    # 密钥生成
    keygen_speedup_no_parallel = basic_results['keygen_time'] / optimized_no_parallel['keygen_time']
    keygen_speedup_with_parallel = basic_results['keygen_time'] / optimized_with_parallel['keygen_time']
    print(f"{'密钥生成时间':<15} {basic_results['keygen_time']:.4f}s    {optimized_no_parallel['keygen_time']:.4f}s    {optimized_with_parallel['keygen_time']:.4f}s    {keygen_speedup_no_parallel:.2f}x          {keygen_speedup_with_parallel:.2f}x")
    
    # 签名
    sign_speedup_no_parallel = basic_results['avg_sign_time'] / optimized_no_parallel['avg_sign_time']
    sign_speedup_with_parallel = basic_results['avg_sign_time'] / optimized_with_parallel['avg_sign_time']
    print(f"{'平均签名时间':<15} {basic_results['avg_sign_time']:.6f}s  {optimized_no_parallel['avg_sign_time']:.6f}s  {optimized_with_parallel['avg_sign_time']:.6f}s  {sign_speedup_no_parallel:.2f}x          {sign_speedup_with_parallel:.2f}x")
    
    # 验证
    verify_speedup_no_parallel = basic_results['avg_verify_time'] / optimized_no_parallel['avg_verify_time']
    verify_speedup_with_parallel = basic_results['avg_verify_time'] / optimized_with_parallel['avg_verify_time']
    print(f"{'平均验证时间':<15} {basic_results['avg_verify_time']:.6f}s  {optimized_no_parallel['avg_verify_time']:.6f}s  {optimized_with_parallel['avg_verify_time']:.6f}s  {verify_speedup_no_parallel:.2f}x          {verify_speedup_with_parallel:.2f}x")
    
    # 加密
    encrypt_speedup_no_parallel = basic_results['avg_encrypt_time'] / optimized_no_parallel['avg_encrypt_time']
    encrypt_speedup_with_parallel = basic_results['avg_encrypt_time'] / optimized_with_parallel['avg_encrypt_time']
    print(f"{'平均加密时间':<15} {basic_results['avg_encrypt_time']:.6f}s  {optimized_no_parallel['avg_encrypt_time']:.6f}s  {optimized_with_parallel['avg_encrypt_time']:.6f}s  {encrypt_speedup_no_parallel:.2f}x          {encrypt_speedup_with_parallel:.2f}x")
    
    # 解密
    decrypt_speedup_no_parallel = basic_results['avg_decrypt_time'] / optimized_no_parallel['avg_decrypt_time']
    decrypt_speedup_with_parallel = basic_results['avg_decrypt_time'] / optimized_with_parallel['avg_decrypt_time']
    print(f"{'平均解密时间':<15} {basic_results['avg_decrypt_time']:.6f}s  {optimized_no_parallel['avg_decrypt_time']:.6f}s  {optimized_with_parallel['avg_decrypt_time']:.6f}s  {decrypt_speedup_no_parallel:.2f}x          {decrypt_speedup_with_parallel:.2f}x")
    
    # 总时间
    total_speedup_no_parallel = basic_results['total_time'] / optimized_no_parallel['total_time']
    total_speedup_with_parallel = basic_results['total_time'] / optimized_with_parallel['total_time']
    print(f"{'总时间':<15} {basic_results['total_time']:.4f}s    {optimized_no_parallel['total_time']:.4f}s    {optimized_with_parallel['total_time']:.4f}s    {total_speedup_no_parallel:.2f}x          {total_speedup_with_parallel:.2f}x")
    
    return  # 添加return语句，避免执行后续代码


def main():
    """主函数"""
    # 增加迭代次数以获得更稳定的结果
    # 可选: 测试不同并行选项
    compare_performance(iterations=1000, test_parallel_options=True)


if __name__ == "__main__":
    main()