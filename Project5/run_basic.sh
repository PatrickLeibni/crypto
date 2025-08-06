#!/bin/bash

# SM2基础实现与算法优化演示脚本
# SM2 Basic Implementation and Algorithm Optimization Demo

echo "=========================================="
echo "SM2基础实现与算法优化演示"
echo "SM2 Basic Implementation and Algorithm Optimization"
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
echo "1. SM2基础实现测试"
echo "==========================================${NC}"

echo -e "${BLUE}运行SM2基础实现...${NC}"
cd src && python3 sm2_basic.py
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ SM2基础实现测试完成${NC}"
else
    echo -e "${RED}✗ SM2基础实现测试失败${NC}"
fi

echo -e "${YELLOW}=========================================="
echo "2. SM2优化实现测试"
echo "==========================================${NC}"

echo -e "${BLUE}运行SM2优化实现...${NC}"
python3 sm2_optimized.py
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ SM2优化实现测试完成${NC}"
else
    echo -e "${RED}✗ SM2优化实现测试失败${NC}"
fi

echo -e "${YELLOW}=========================================="
echo "3. 性能对比分析"
echo "==========================================${NC}"

echo -e "${BLUE}生成性能对比报告...${NC}"
cat > ../results/basic_vs_optimized.md << EOF
# SM2基础实现 vs 优化实现性能对比

## 测试环境
- 日期: $(date)
- Python版本: $(python3 --version)
- 系统: $(uname -a)

## 性能指标
- 密钥生成时间
- 签名生成时间
- 签名验证时间
- 加密时间
- 解密时间

## 优化技术
1. NAF（Non-Adjacent Form）表示法
2. 椭圆曲线点预计算
3. 并行化计算
4. 内存使用优化

## 测试结果
详细结果请查看日志文件。
EOF

echo -e "${GREEN}✓ 性能对比报告已生成${NC}"

echo -e "${YELLOW}=========================================="
echo "演示完成！"
echo "==========================================${NC}"

echo -e "${BLUE}生成的文件：${NC}"
echo "  - 日志: logs/"
echo "  - 结果: results/"
echo "  - 性能报告: results/basic_vs_optimized.md"

echo -e "${GREEN}SM2基础实现与算法优化演示完成！${NC}" 