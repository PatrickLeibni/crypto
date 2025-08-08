# 数字水印系统 Makefile

.PHONY: help install test clean demo cli docs output clean-output run-demo status check-deps benchmark

# 默认目标
help:
	@echo "数字水印系统 - 可用命令:"
	@echo ""
	@echo "基础命令:"
	@echo "  install      - 安装依赖"
	@echo "  test         - 运行测试"
	@echo "  demo         - 运行演示"
	@echo "  cli          - 运行命令行工具"
	@echo "  clean        - 清理临时文件"
	@echo "  docs         - 生成文档"
	@echo "  build        - 构建项目"
	@echo "  install-dev  - 安装开发依赖"
	@echo ""
	@echo "演示和测试:"
	@echo "  run-demo     - 运行完整演示（推荐）"
	@echo "  create-sample - 创建示例图片"
	@echo "  embed-example - 嵌入水印示例"
	@echo "  detect-example - 检测水印示例"
	@echo "  test-example - 鲁棒性测试示例"
	@echo "  full-demo    - 完整演示流程"
	@echo "  benchmark    - 性能测试"
	@echo ""
	@echo "文件管理:"
	@echo "  output       - 查看输出文件"
	@echo "  clean-output - 清理输出文件"
	@echo "  status       - 查看项目状态"
	@echo "  check-deps   - 检查依赖"

# 安装依赖
install:
	pip install -r requirements.txt

# 安装开发依赖
install-dev:
	pip install -r requirements.txt

# 运行测试
test:
	python -m pytest tests/ -v --cov=src --cov-report=html

# 运行演示
demo:
	python examples/demo.py

# 运行命令行工具
cli:
	python cli.py --help

# 清理临时文件
clean:
	find . -type f -name "*.pyc" -delete
	find . -type d -name "__pycache__" -delete
	find . -type f -name "*.log" -delete
	find . -type f -name "*.tmp" -delete
	rm -rf data/results/*
	rm -rf .pytest_cache
	rm -rf htmlcov

# 生成文档
docs:
	python -c "import pydoc; pydoc.writedoc('src.watermark')"
	python -c "import pydoc; pydoc.writedoc('src.robustness_test')"

# 构建项目
build:
	python setup.py build

# 安装项目
install-package:
	python setup.py install

# 代码格式化
format:
	black src/ tests/ examples/ cli.py utils.py config.py



# 创建示例图片
create-sample:
	python cli.py create-sample -o output/sample.jpg

# 嵌入水印示例
embed-example:
	python cli.py embed -i output/sample.jpg -o output/watermarked.jpg -m "Test watermark"

# 检测水印示例
detect-example:
	python cli.py detect -i output/watermarked.jpg -m "Test watermark"

# 鲁棒性测试示例
test-example:
	python cli.py test -i output/watermarked.jpg -m "Test watermark" --report output/report.txt --plot output/plot.png

# 完整演示
full-demo: create-sample embed-example detect-example test-example
	@echo "完整演示完成!"

# 性能测试
benchmark:
	python -c "from utils import benchmark_performance; from src.watermark import DigitalWatermark; wm = DigitalWatermark(); result, time = benchmark_performance(wm.embed_watermark, 'output/sample.jpg', message='benchmark'); print(f'嵌入时间: {time:.4f}秒')"

# 检查依赖
check-deps:
	python -c "import cv2, numpy, PIL, matplotlib; print('所有依赖已安装')"

# 项目状态
status:
	@echo "项目状态:"
	@echo "  源代码: $(shell find src/ -name '*.py' | wc -l) 个文件"
	@echo "  测试: $(shell find tests/ -name '*.py' | wc -l) 个文件"
	@echo "  示例: $(shell find examples/ -name '*.py' | wc -l) 个文件"
	@echo "  输出文件: $(shell find output/ -name '*' 2>/dev/null | wc -l) 个文件"

# 查看输出文件
output:
	@echo "输出文件夹内容:"
	@ls -la output/ 2>/dev/null || echo "output文件夹为空"

# 清理输出文件
clean-output:
	@echo "清理输出文件..."
	@rm -rf output/*
	@echo "输出文件已清理"

# 运行完整演示
run-demo:
	@echo "运行完整演示..."
	python examples/demo.py
	@echo "  数据文件: $(shell find data/ -name '*' | wc -l) 个文件" 
