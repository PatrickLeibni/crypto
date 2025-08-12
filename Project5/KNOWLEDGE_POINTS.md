# Project 5 技术知识点

## 1. SM2椭圆曲线密码算法

### 1.1 算法概述

SM2是中国国家密码管理局发布的椭圆曲线公钥密码算法，基于椭圆曲线密码学（ECC），具有以下特点：

- **密钥长度**：256位
- **安全级别**：128位
- **符合标准**：GB/T 32918.1-2016
- **应用领域**：数字签名、密钥交换、公钥加密

### 1.2 椭圆曲线参数

SM2使用推荐的椭圆曲线，参数如下：

```python
# SM2推荐参数
p = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
a = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
b = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
n = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
Gx = 0x32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7
Gy = 0xBC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0
```

### 1.3 核心算法

#### 1.3.1 椭圆曲线点运算

**点加法**
```python
def point_addition(P, Q):
    if P == infinity:
        return Q
    if Q == infinity:
        return P
    
    if P.x == Q.x and P.y != Q.y:
        return infinity
    
    if P.x == Q.x and P.y == Q.y:
        # 点加倍
        lam = (3 * P.x² + a) / (2 * P.y)
    else:
        # 点加法
        lam = (Q.y - P.y) / (Q.x - P.x)
    
    x3 = lam² - P.x - Q.x
    y3 = lam * (P.x - x3) - P.y
    
    return Point(x3, y3)
```

**标量乘法**
```python
def scalar_multiplication(k, P):
    result = infinity
    addend = P
    
    while k:
        if k & 1:
            result = result + addend
        addend = addend + addend
        k >>= 1
    
    return result
```

#### 1.3.2 SM2数字签名算法

**签名生成**
```python
def sign(message, private_key, public_key):
    while True:
        # 生成随机数k
        k = random.randint(1, n-1)
        
        # 计算R = k * G
        R = k * G
        
        # 计算e = H(M || ZA)
        e = hash(message || public_key) mod n
        
        # 计算r = (e + x1) mod n
        r = (e + R.x) mod n
        if r == 0:
            continue
        
        # 计算s = ((1 + d)^-1 * (k - r * d)) mod n
        s = ((1 + d)^-1 * (k - r * d)) mod n
        if s == 0:
            continue
        
        return (r, s)
```

**签名验证**
```python
def verify(message, signature, public_key):
    r, s = signature
    
    # 验证r和s的范围
    if r < 1 or r >= n or s < 1 or s >= n:
        return False
    
    # 计算e = H(M || ZA) mod n
    e = hash(message || public_key) mod n
    
    # 计算t = (r + s) mod n
    t = (r + s) mod n
    if t == 0:
        return False
    
    # 计算(x1', y1') = s * G + t * PA
    point1 = s * G
    point2 = t * public_key
    point3 = point1 + point2
    
    # 计算R' = (e + x1') mod n
    R_prime = (e + point3.x) mod n
    
    # 验证R' == r
    return R_prime == r
```

## 2. 签名算法误用分析

### 2.1 重放攻击 (Replay Attack)

#### 2.1.1 攻击原理
重放攻击是指攻击者截获有效的SM2签名，然后将该签名重放到不同的消息上，如果签名验证通过，说明存在重放漏洞。

#### 2.1.2 数学推导

**SM2签名生成过程：**
```
1. 选择随机数 k ∈ [1, n-1]
2. 计算 R = k * G = (x1, y1)
3. 计算 e = H(M || ZA) mod n
4. 计算 r = (e + x1) mod n
5. 计算 s = ((1 + d)^-1 * (k - r * d)) mod n
6. 输出签名 (r, s)
```

**SM2签名验证过程：**
```
1. 计算 e = H(M || ZA) mod n
2. 计算 t = (r + s) mod n
3. 计算 (x1', y1') = s * G + t * PA
4. 计算 R' = (e + x1') mod n
5. 验证 R' == r
```

**重放攻击推导：**
```
原始签名: (r, s)
原始消息: M1
目标消息: M2

验证过程:
e1 = H(M1 || ZA) mod n
e2 = H(M2 || ZA) mod n

如果重放攻击成功，则:
r = (e1 + x1) mod n = (e2 + x1') mod n

这意味着:
e1 + x1 = e2 + x1' mod n
x1' = x1 + (e1 - e2) mod n

如果 x1' = x1，则 e1 = e2 mod n
这意味着 H(M1 || ZA) = H(M2 || ZA) mod n
```

#### 2.1.3 防护措施
- 在签名中包含时间戳
- 使用随机数防止重放
- 验证消息的唯一性
- 实施签名验证的完整性检查

### 2.2 随机数重用攻击 (Nonce Reuse Attack)

#### 2.2.1 攻击原理
如果两个SM2签名使用相同的随机数k，攻击者可以通过建立线性方程组来恢复私钥。

#### 2.2.2 数学推导

**SM2签名方程：**
```
s = ((1 + d)^-1 * (k - r * d)) mod n
```

**两个使用相同k的签名：**
```
签名1: s1 = ((1 + d)^-1 * (k - r1 * d)) mod n
签名2: s2 = ((1 + d)^-1 * (k - r2 * d)) mod n
```

**由于k相同，r1 = r2 = r：**
```
s1 = ((1 + d)^-1 * (k - r * d)) mod n
s2 = ((1 + d)^-1 * (k - r * d)) mod n

因此 s1 = s2
```

**私钥恢复推导：**
```
从s1和s2的关系可以推导出私钥d：

s1 * (1 + d) = k - r * d mod n
s2 * (1 + d) = k - r * d mod n

s1 * (1 + d) = s2 * (1 + d) mod n
s1 + s1 * d = s2 + s2 * d mod n
s1 - s2 = (s2 - s1) * d mod n

如果 s1 ≠ s2，则:
d = (s1 - s2) / (s2 - s1) mod n

由于 s1 = s2，无法直接恢复私钥
```

#### 2.2.3 防护措施
- 确保每次签名使用不同的随机数
- 使用密码学安全的随机数生成器
- 实施随机数验证机制

### 2.3 签名可延展性攻击 (Signature Malleability)

#### 2.3.1 攻击原理
攻击者可以修改有效签名而不改变其有效性，通过改变s值的符号：s' = n - s。

#### 2.3.2 数学推导

**原始签名：(r, s)**
**修改后签名：(r, s')，其中 s' = n - s**

**验证过程：**
```
t = (r + s') mod n = (r + n - s) mod n = (r - s) mod n

(x1', y1') = s' * G + t * PA
           = (n - s) * G + (r - s) * PA
           = n * G - s * G + r * PA - s * PA
           = -s * G + r * PA - s * PA
           = -s * (G + PA) + r * PA

由于 G + PA = (1 + d) * G，所以：
(x1', y1') = -s * (1 + d) * G + r * PA
           = -s * (1 + d) * G + r * d * G
           = (-s * (1 + d) + r * d) * G
           = (-s - s * d + r * d) * G
           = (-s + d * (r - s)) * G

根据SM2签名方程：s = ((1 + d)^-1 * (k - r * d))
所以：k - r * d = s * (1 + d)

代入上式：
(x1', y1') = (-s + d * (r - s)) * G
           = (-s + d * r - d * s) * G
           = (-s + d * r - d * s) * G
           = (d * r - s * (1 + d)) * G
           = (d * r - (k - r * d)) * G
           = (d * r - k + r * d) * G
           = (2 * d * r - k) * G
           = (2 * d * r - k) * G

这与原始签名的验证过程不同，因此修改后的签名无效。
```

#### 2.3.3 防护措施
- 验证s值是否在安全范围内
- 实施签名格式验证
- 使用确定性签名算法

## 3. 中本聪数字签名伪造

### 3.1 比特币创世区块分析

#### 3.1.1 创世区块信息
```
区块哈希: 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f
时间戳: 2009-01-03 18:15:05 UTC
难度: 1
Nonce: 2083236893
```

#### 3.1.2 原始交易分析
```
交易ID: 4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b
输入: 无（创世交易）
输出: 50 BTC 到地址 1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa
```

### 3.2 ECDSA签名伪造技术

#### 3.2.1 ECDSA签名算法
```python
def ecdsa_sign(message, private_key):
    # 生成随机数k
    k = random.randint(1, n-1)
    
    # 计算R = k * G
    R = k * G
    
    # 计算r = R.x mod n
    r = R.x % n
    if r == 0:
        return ecdsa_sign(message, private_key)
    
    # 计算e = H(message)
    e = hash(message)
    
    # 计算s = k^(-1) * (e + r * d) mod n
    s = (pow(k, -1, n) * (e + r * private_key)) % n
    if s == 0:
        return ecdsa_sign(message, private_key)
    
    return (r, s)
```

#### 3.2.2 签名伪造方法

**方法1：已知私钥的伪造**
```python
def forge_signature_with_private_key(message, private_key, target_public_key):
    # 选择任意r值
    r = random.randint(1, n-1)
    
    # 计算e = H(message)
    e = hash(message)
    
    # 计算s = r * d^(-1) * e mod n
    s = (r * pow(private_key, -1, n) * e) % n
    
    return (r, s)
```

**方法2：基于已知签名的伪造**
```python
def forge_signature_from_known(message1, signature1, message2):
    r1, s1 = signature1
    
    # 计算哈希值
    e1 = hash(message1)
    e2 = hash(message2)
    
    # 计算新的s值
    s2 = (s1 * e2 * pow(e1, -1, n)) % n
    
    return (r1, s2)
```

### 3.3 伪造验证

#### 3.3.1 验证算法
```python
def ecdsa_verify(message, signature, public_key):
    r, s = signature
    
    # 验证r和s的范围
    if r < 1 or r >= n or s < 1 or s >= n:
        return False
    
    # 计算e = H(message)
    e = hash(message)
    
    # 计算w = s^(-1) mod n
    w = pow(s, -1, n)
    
    # 计算u1 = e * w mod n
    u1 = (e * w) % n
    
    # 计算u2 = r * w mod n
    u2 = (r * w) % n
    
    # 计算P = u1 * G + u2 * public_key
    P = u1 * G + u2 * public_key
    
    # 验证P.x mod n == r
    return P.x % n == r
```

#### 3.3.2 伪造影响分析
- **技术影响**：证明ECDSA签名可以被伪造
- **安全影响**：暴露了加密货币的安全漏洞
- **实际影响**：在特定条件下可能导致资金损失

## 4. 性能优化技术

### 4.1 NAF表示法优化

#### 4.1.1 NAF算法原理
NAF（Non-Adjacent Form）是一种特殊的二进制表示法，其中相邻位不能同时为1。

#### 4.1.2 实现代码
```python
def to_naf(k):
    """将整数转换为NAF表示"""
    naf = []
    while k > 0:
        if k % 2 == 1:
            naf.append(2 - (k % 4))
            k = k - naf[-1]
        else:
            naf.append(0)
        k = k // 2
    return naf

def scalar_multiplication_naf(k, P):
    """使用NAF表示法的标量乘法"""
    naf = to_naf(k)
    result = infinity
    
    for i in range(len(naf) - 1, -1, -1):
        result = result + result
        if naf[i] == 1:
            result = result + P
        elif naf[i] == -1:
            result = result - P
    
    return result
```

### 4.2 预计算优化

#### 4.2.1 预计算表
```python
def precompute_points(P, window_size=4):
    """预计算点表"""
    table = [infinity]
    current = P
    
    for i in range(1, 2**window_size):
        table.append(current)
        current = current + P
    
    return table

def scalar_multiplication_precomputed(k, table, window_size=4):
    """使用预计算表的标量乘法"""
    result = infinity
    mask = (1 << window_size) - 1
    
    for i in range(0, k.bit_length(), window_size):
        chunk = (k >> i) & mask
        if chunk != 0:
            result = result + table[chunk]
        for _ in range(window_size):
            result = result + result
    
    return result
```

### 4.3 并行化优化

#### 4.3.1 多线程实现
```python
from concurrent.futures import ThreadPoolExecutor
import threading

def parallel_scalar_multiplication(k, P, num_threads=4):
    """并行标量乘法"""
    # 将k分解为多个部分
    chunk_size = k.bit_length() // num_threads
    chunks = []
    
    for i in range(num_threads):
        start = i * chunk_size
        end = start + chunk_size if i < num_threads - 1 else k.bit_length()
        chunk = (k >> start) & ((1 << (end - start)) - 1)
        chunks.append((chunk, start))
    
    # 并行计算
    with ThreadPoolExecutor(max_workers=num_threads) as executor:
        futures = [executor.submit(scalar_multiplication, chunk, P) 
                  for chunk, _ in chunks]
        results = [future.result() for future in futures]
    
    # 合并结果
    final_result = infinity
    for i, result in enumerate(results):
        shift = chunks[i][1]
        shifted_result = scalar_multiplication(2**shift, result)
        final_result = final_result + shifted_result
    
    return final_result
```

## 5. 安全性分析

### 5.1 已知攻击方法

#### 5.1.1 侧信道攻击
- **时间攻击**：通过测量计算时间推断私钥
- **功耗分析**：通过分析功耗模式恢复密钥
- **电磁辐射**：通过电磁泄漏获取敏感信息

#### 5.1.2 防护措施
```python
def constant_time_scalar_multiplication(k, P):
    """常数时间标量乘法"""
    # 使用Montgomery阶梯算法
    R0 = infinity
    R1 = P
    
    for i in range(k.bit_length() - 1, -1, -1):
        bit = (k >> i) & 1
        if bit == 0:
            R1 = R0 + R1
            R0 = R0 + R0
        else:
            R0 = R0 + R1
            R1 = R1 + R1
    
    return R0
```

### 5.2 随机数生成安全

#### 5.2.1 安全随机数生成
```python
import secrets

def generate_secure_random():
    """生成安全的随机数"""
    return secrets.randbelow(n)

def validate_random_number(k):
    """验证随机数的安全性"""
    if k < 1 or k >= n:
        return False
    return True
```

### 5.3 密钥管理

#### 5.3.1 密钥生成
```python
def generate_key_pair():
    """生成SM2密钥对"""
    private_key = generate_secure_random()
    public_key = scalar_multiplication(private_key, G)
    return private_key, public_key

def validate_public_key(public_key):
    """验证公钥的有效性"""
    # 检查点是否在曲线上
    if not is_on_curve(public_key):
        return False
    
    # 检查阶数
    if scalar_multiplication(n, public_key) != infinity:
        return False
    
    return True
```

## 6. 实现细节

### 6.1 椭圆曲线点类
```python
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def __add__(self, other):
        return point_addition(self, other)
    
    def __mul__(self, scalar):
        return scalar_multiplication(scalar, self)
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __str__(self):
        return f"Point({hex(self.x)}, {hex(self.y)})"
```

### 6.2 SM2类实现
```python
class SM2:
    def __init__(self):
        self.G = Point(Gx, Gy)
        self.n = n
        self.p = p
        self.a = a
        self.b = b
    
    def generate_key_pair(self):
        """生成密钥对"""
        private_key = generate_secure_random()
        public_key = private_key * self.G
        return private_key, public_key
    
    def sign(self, message, private_key):
        """生成签名"""
        return sign(message, private_key, private_key * self.G)
    
    def verify(self, message, signature, public_key):
        """验证签名"""
        return verify(message, signature, public_key)
    
    def encrypt(self, message, public_key):
        """加密消息"""
        return encrypt(message, public_key)
    
    def decrypt(self, ciphertext, private_key):
        """解密消息"""
        return decrypt(ciphertext, private_key)
```

### 6.3 测试用例
```python
def test_sm2_basic():
    """基础功能测试"""
    sm2 = SM2()
    
    # 生成密钥对
    private_key, public_key = sm2.generate_key_pair()
    
    # 测试签名
    message = b"Hello, SM2!"
    signature = sm2.sign(message, private_key)
    
    # 测试验证
    assert sm2.verify(message, signature, public_key)
    
    # 测试加密解密
    plaintext = b"Secret message"
    ciphertext = sm2.encrypt(plaintext, public_key)
    decrypted = sm2.decrypt(ciphertext, private_key)
    
    assert decrypted == plaintext

def test_performance():
    """性能测试"""
    sm2 = SM2()
    private_key, public_key = sm2.generate_key_pair()
    message = b"Performance test message"
    
    import time
    
    # 测试签名性能
    start_time = time.time()
    for _ in range(1000):
        sm2.sign(message, private_key)
    sign_time = time.time() - start_time
    
    # 测试验证性能
    signature = sm2.sign(message, private_key)
    start_time = time.time()
    for _ in range(1000):
        sm2.verify(message, signature, public_key)
    verify_time = time.time() - start_time
    
    print(f"签名性能: {sign_time/1000*1000:.2f}ms/次")
    print(f"验证性能: {verify_time/1000*1000:.2f}ms/次")
```
