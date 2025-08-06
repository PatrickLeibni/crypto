#!/bin/bash

# SM2签名算法误用POC验证脚本
# SM2 Signature Algorithm Misuse POC Verification

echo "=========================================="
echo "SM2签名算法误用POC验证"
echo "SM2 Signature Algorithm Misuse POC Verification"
echo "=========================================="

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 检查Python环境
echo -e "${BLUE}检查Python环境...${NC}"
python3 --version
if [ $? -ne 0 ]; then
    echo -e "${RED}Python3未安装。请安装Python 3.8+${NC}"
    exit 1
fi

# 检查依赖包
echo -e "${BLUE}检查依赖包...${NC}"
python3 -c "import gmpy2, numpy, ecdsa" 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${YELLOW}安装依赖包...${NC}"
    pip3 install -r requirements.txt
fi

# 创建必要的目录
echo -e "${BLUE}设置项目结构...${NC}"
mkdir -p logs
mkdir -p results

echo -e "${YELLOW}=========================================="
echo "基于20250713-wen-sm2-public.pdf的签名算法误用分析"
echo "==========================================${NC}"

echo -e "${BLUE}运行签名算法误用POC验证...${NC}"
cd src && python3 sm2_signature_misuse.py
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ 签名算法误用POC验证完成${NC}"
else
    echo -e "${RED}✗ 签名算法误用POC验证失败${NC}"
fi

echo -e "${YELLOW}=========================================="
echo "攻击类型分析"
echo "==========================================${NC}"

echo -e "${BLUE}1. 重放攻击（Replay Attack）${NC}"
echo "   - 攻击原理：截获有效签名并重放到不同消息"
echo "   - 防护措施：在签名中包含时间戳或随机数"
echo "   - 验证结果：请查看上述输出"

echo -e "${BLUE}2. 随机数重用攻击（Nonce Reuse Attack）${NC}"
echo "   - 攻击原理：两个签名使用相同随机数k"
echo "   - 防护措施：确保每次签名使用不同随机数"
echo "   - 验证结果：请查看上述输出"

echo -e "${BLUE}3. 签名可延展性攻击（Signature Malleability）${NC}"
echo "   - 攻击原理：修改有效签名而不影响验证结果"
echo "   - 防护措施：在签名中包含消息哈希"
echo "   - 验证结果：请查看上述输出"

echo -e "${BLUE}4. 密钥恢复攻击（Key Recovery Attack）${NC}"
echo "   - 攻击原理：利用已知私钥部分信息恢复完整私钥"
echo "   - 防护措施：保护私钥完整性，使用硬件安全模块"
echo "   - 验证结果：请查看上述输出"

echo -e "${YELLOW}=========================================="
echo "生成安全分析报告"
echo "==========================================${NC}"

echo -e "${BLUE}生成安全分析报告...${NC}"
cat > ../results/signature_misuse_analysis.md << EOF
# SM2签名算法误用安全分析报告

## 分析背景
基于20250713-wen-sm2-public.pdf中提到的签名算法误用问题

## 攻击类型分析

### 1. 重放攻击（Replay Attack）
- **攻击原理**：攻击者截获有效的SM2签名，将签名重放到不同的消息上
- **风险等级**：中等
- **防护措施**：
  - 在签名中包含时间戳
  - 使用随机数防止重放
  - 实施签名唯一性检查

### 2. 随机数重用攻击（Nonce Reuse Attack）
- **攻击原理**：如果两个签名使用相同的随机数k，可以建立线性方程组求解私钥
- **风险等级**：高
- **防护措施**：
  - 确保每次签名使用不同的随机数
  - 使用安全的随机数生成器
  - 实施随机数验证机制

### 3. 签名可延展性攻击（Signature Malleability）
- **攻击原理**：攻击者可以修改有效签名而不影响验证结果
- **风险等级**：中等
- **防护措施**：
  - 在签名中包含消息哈希
  - 使用确定性签名算法
  - 实施签名唯一性验证

### 4. 密钥恢复攻击（Key Recovery Attack）
- **攻击原理**：利用已知的私钥部分信息恢复完整私钥
- **风险等级**：高
- **防护措施**：
  - 保护私钥的完整性
  - 使用硬件安全模块
  - 定期更新密钥对

## 安全建议

### 算法层面
1. 使用安全的随机数生成器
2. 实施签名唯一性检查
3. 在签名中包含时间戳或随机数
4. 使用确定性签名算法

### 实现层面
1. 验证所有输入参数
2. 实施签名验证完整性检查
3. 使用硬件安全模块保护私钥
4. 定期进行安全审计

### 协议层面
1. 在交易中包含时间戳
2. 使用序列号防止重放
3. 实施交易确认机制
4. 建立监管框架

## 测试环境
- 日期: $(date)
- Python版本: $(python3 --version)
- 系统: $(uname -a)

## 测试结果
详细结果请查看日志文件。
EOF

echo -e "${GREEN}✓ 安全分析报告已生成${NC}"

echo -e "${YELLOW}=========================================="
echo "演示完成！"
echo "==========================================${NC}"

echo -e "${BLUE}生成的文件：${NC}"
echo "  - 日志: logs/"
echo "  - 结果: results/"
echo "  - 安全分析报告: results/signature_misuse_analysis.md"

echo -e "${GREEN}SM2签名算法误用POC验证完成！${NC}" 