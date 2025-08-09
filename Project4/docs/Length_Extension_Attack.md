# SM3 长度扩展攻击详细说明

## 概述

长度扩展攻击是一种针对基于Merkle-Damgård结构的哈希函数的攻击方法。由于SM3采用Merkle-Damgård结构，理论上容易受到此类攻击。

## 攻击原理

### 1. Merkle-Damgård结构弱点

Merkle-Damgård结构的基本流程：
1. 将消息分割成固定长度的块
2. 对每个块应用压缩函数
3. 将前一个块的输出作为下一个块的输入

这种结构的弱点在于：如果攻击者知道某个消息的哈希值，就可以构造一个扩展消息，而不需要知道原始消息的内容。

### 2. 攻击步骤

#### 步骤1：获取原始哈希值
```
原始消息: M
原始哈希: H = Hash(M)
```

#### 步骤2：构造填充
计算原始消息的填充：
```
填充长度 = 512 - (消息长度 % 512) - 65
```

#### 步骤3：构造扩展消息
```
扩展消息 = M || 填充 || 扩展内容
```

#### 步骤4：计算新哈希
```
新哈希 = Hash(扩展消息)
```

### 3. 攻击实现

```c
int sm3_length_extension_attack(
    const uint8_t *original_digest,
    const uint8_t *original_message,
    size_t original_len,
    const uint8_t *extension,
    size_t extension_len,
    uint8_t *new_digest
) {
    // 1. 计算填充长度
    int padding_len = sm3_calculate_padding_length(original_len);
    
    // 2. 构造完整消息
    uint8_t *full_message = malloc(original_len + padding_len + extension_len);
    memcpy(full_message, original_message, original_len);
    
    // 添加填充
    full_message[original_len] = 0x80;
    memset(full_message + original_len + 1, 0, padding_len - 8 - 1);
    
    // 添加长度（64位，小端序）
    uint64_t bit_len = original_len * 8;
    for (int i = 0; i < 8; i++) {
        full_message[original_len + padding_len - 8 + i] = (bit_len >> (i * 8)) & 0xFF;
    }
    
    // 添加扩展内容
    memcpy(full_message + original_len + padding_len, extension, extension_len);
    
    // 3. 计算新哈希
    sm3_hash(full_message, original_len + padding_len + extension_len, new_digest);
    
    free(full_message);
    return 0;
}
```

## 攻击验证

### 1. 验证方法

为了验证攻击的正确性，我们需要：
1. 计算原始消息的哈希
2. 执行长度扩展攻击
3. 计算完整消息的哈希
4. 比较两个结果

### 2. 验证代码

```c
void verify_length_extension_attack() {
    const char *original_message = "Hello, World!";
    const char *extension = "Attack!";
    
    uint8_t original_digest[SM3_DIGEST_SIZE];
    uint8_t attack_digest[SM3_DIGEST_SIZE];
    uint8_t verification_digest[SM3_DIGEST_SIZE];
    
    // 计算原始哈希
    sm3_hash((uint8_t*)original_message, strlen(original_message), original_digest);
    
    // 执行攻击
    sm3_length_extension_attack(
        original_digest,
        (uint8_t*)original_message,
        strlen(original_message),
        (uint8_t*)extension,
        strlen(extension),
        attack_digest
    );
    
    // 计算验证哈希
    char full_message[1024];
    int padding_len = sm3_calculate_padding_length(strlen(original_message));
    int total_len = strlen(original_message) + padding_len + strlen(extension);
    
    memcpy(full_message, original_message, strlen(original_message));
    full_message[strlen(original_message)] = 0x80;
    memset(full_message + strlen(original_message) + 1, 0, padding_len - 8 - 1);
    
    uint64_t bit_len = strlen(original_message) * 8;
    for (int i = 0; i < 8; i++) {
        full_message[strlen(original_message) + padding_len - 8 + i] = (bit_len >> (i * 8)) & 0xFF;
    }
    
    memcpy(full_message + strlen(original_message) + padding_len, extension, strlen(extension));
    
    sm3_hash((uint8_t*)full_message, total_len, verification_digest);
    
    // 比较结果
    if (memcmp(attack_digest, verification_digest, SM3_DIGEST_SIZE) == 0) {
        printf("攻击验证成功！\n");
    } else {
        printf("攻击验证失败！\n");
    }
}
```

## 防护措施

### 1. HMAC构造

使用HMAC可以防止长度扩展攻击：

```c
uint8_t hmac_sm3(const uint8_t *key, size_t key_len, 
                  const uint8_t *message, size_t message_len, 
                  uint8_t *digest) {
    uint8_t ipad[64], opad[64];
    uint8_t key_pad[64];
    
    // 准备密钥
    if (key_len > 64) {
        sm3_hash(key, key_len, key_pad);
        key_len = SM3_DIGEST_SIZE;
    } else {
        memcpy(key_pad, key, key_len);
    }
    
    // 填充密钥
    memset(key_pad + key_len, 0, 64 - key_len);
    
    // 计算ipad和opad
    for (int i = 0; i < 64; i++) {
        ipad[i] = key_pad[i] ^ 0x36;
        opad[i] = key_pad[i] ^ 0x5C;
    }
    
    // 计算内哈希
    uint8_t inner_digest[SM3_DIGEST_SIZE];
    sm3_hash(ipad, 64, inner_digest);
    sm3_hash(message, message_len, inner_digest);
    
    // 计算外哈希
    sm3_hash(opad, 64, digest);
    sm3_hash(inner_digest, SM3_DIGEST_SIZE, digest);
    
    return 0;
}
```

### 2. 使用盐值

在哈希计算中加入随机盐值：

```c
uint8_t salted_sm3(const uint8_t *message, size_t message_len,
                   const uint8_t *salt, size_t salt_len,
                   uint8_t *digest) {
    uint8_t *salted_message = malloc(message_len + salt_len);
    memcpy(salted_message, salt, salt_len);
    memcpy(salted_message + salt_len, message, message_len);
    
    sm3_hash(salted_message, message_len + salt_len, digest);
    free(salted_message);
    
    return 0;
}
```

### 3. 使用SHAKE构造

采用SHAKE等可扩展输出函数：

```c
// 伪代码示例
uint8_t shake_sm3(const uint8_t *message, size_t message_len,
                  uint8_t *output, size_t output_len) {
    // 使用SM3作为基础函数构造SHAKE
    // 实现可扩展输出
}
```

## 实际应用场景

### 1. 数字签名

在数字签名中，长度扩展攻击可能导致伪造签名：

```c
// 攻击者可能构造的恶意消息
原始消息: "用户同意转账100元"
攻击者构造: "用户同意转账100元" + 填充 + "转账1000000元"
```

### 2. 消息认证码

在MAC系统中，攻击者可能构造新的有效MAC：

```c
// 攻击者知道MAC(key, "message")
// 可以构造MAC(key, "message" + 填充 + "extension")
```

### 3. 密码存储

在密码存储中，攻击者可能利用哈希值构造新的有效密码哈希。

## 测试用例

### 1. 基本测试

```c
void test_basic_length_extension() {
    const char *messages[] = {"a", "abc", "Hello", "Test message"};
    const char *extensions[] = {"X", "123", "Attack", "Extension"};
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            test_single_attack(messages[i], extensions[j]);
        }
    }
}
```

### 2. 边界测试

```c
void test_edge_cases() {
    // 测试空消息
    test_single_attack("", "extension");
    
    // 测试长消息
    char long_message[1000];
    memset(long_message, 'A', 999);
    long_message[999] = '\0';
    test_single_attack(long_message, "extension");
    
    // 测试空扩展
    test_single_attack("message", "");
}
```

### 3. 性能测试

```c
void test_attack_performance() {
    const int iterations = 10000;
    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        // 执行攻击
        sm3_length_extension_attack(...);
    }
    
    clock_t end = clock();
    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("攻击性能: %.2f 次/秒\n", iterations / time);
}
```

## 总结

长度扩展攻击是Merkle-Damgård结构哈希函数的一个已知弱点。虽然SM3通过特殊的设计增强了抗性，但在实际应用中仍需要采取适当的防护措施，如使用HMAC、盐值或可扩展输出函数。
