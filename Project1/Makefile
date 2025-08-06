# SM4软件实现和优化 Makefile

CC = gcc
CFLAGS = -O3 -march=native -mavx2 -mavx512f -mgfni -Wall -Wextra -I./include
LDFLAGS = 

# 源文件目录
SRCDIR = src
INCDIR = include
DOCSDIR = docs
EXAMPLESDIR = examples
TESTSDIR = tests

# 源文件
SOURCES = $(SRCDIR)/sm4_vprold.c $(SRCDIR)/sm4_gfni.c $(SRCDIR)/sm4_ttable.c \
          $(SRCDIR)/sm4_gcm.c $(SRCDIR)/sm4_aesni.c $(SRCDIR)/sm4_avx512_gfni.c \
          $(SRCDIR)/sm4_avx512_vprold.c $(SRCDIR)/sm4_basic.c $(SRCDIR)/test_sm4.c

# 目标文件
OBJECTS = $(SOURCES:.c=.o)

# 可执行文件
TARGET = sm4_test

# 示例文件
EXAMPLE_SOURCES = $(wildcard $(EXAMPLESDIR)/*.c)
EXAMPLE_TARGETS = $(EXAMPLE_SOURCES:.c=)

# 测试文件
TEST_SOURCES = $(TESTSDIR)/unit_tests.c $(TESTSDIR)/performance_tests.c \
               $(TESTSDIR)/correctness_tests.c $(TESTSDIR)/compatibility_tests.c
TEST_TARGETS = $(TEST_SOURCES:.c=)

# 默认目标
all: $(TARGET)

# 编译目标
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# 编译规则
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# 编译示例
examples: $(EXAMPLE_TARGETS)

$(EXAMPLESDIR)/%: $(EXAMPLESDIR)/%.c $(filter-out $(SRCDIR)/test_sm4.o, $(OBJECTS))
	$(CC) $(CFLAGS) $< $(filter-out $(SRCDIR)/test_sm4.o, $(OBJECTS)) -o $@ $(LDFLAGS)

# 编译测试
tests: $(TEST_TARGETS)

$(TESTSDIR)/%: $(TESTSDIR)/%.c $(OBJECTS)
	$(CC) $(CFLAGS) $< $(filter-out $(SRCDIR)/test_sm4.o, $(OBJECTS)) -o $@ $(LDFLAGS)

# 检查CPU特性
check_cpu:
	@echo "检查CPU特性支持..."
	@echo "AESNI: $(shell grep -o 'aes' /proc/cpuinfo | head -1 || echo '不支持')"
	@echo "AVX-512: $(shell grep -o 'avx512' /proc/cpuinfo | head -1 || echo '不支持')"
	@echo "GFNI: $(shell grep -o 'gfni' /proc/cpuinfo | head -1 || echo '不支持')"
	@echo "VPROLD: $(shell if grep -q 'avx512' /proc/cpuinfo; then echo '支持 (属于AVX-512指令集)'; else echo '不支持'; fi)"
	

# 运行测试
test: $(TARGET)
	./$(TARGET)

# 运行所有测试
test-all: tests
	@echo "运行单元测试..."
	@for test in $(TEST_TARGETS); do \
		echo "运行 $$test..."; \
		$$test; \
	done

# 性能测试
perf: $(TARGET)
	@echo "性能测试结果:"
	@./$(TARGET) | grep -E "MB/s|秒.*MB/s" || echo "未找到性能数据"

# 创建目录结构
setup:
	@echo "创建项目目录结构..."
	@mkdir -p docs examples tests build
	ls -l docs examples tests build
	@echo "项目结构创建完成"

# 安装依赖
install_deps:
	sudo apt-get update
	sudo apt-get install -y build-essential

# 安装开发工具
install_dev_tools:
	@echo "安装开发工具..."
	@sudo apt-get update
	@sudo apt-get install -y doxygen clang-format cppcheck
	@echo "开发工具安装完成"

# 静态库
lib: $(OBJECTS)
	ar rcs libsm4.a $(filter-out $(SRCDIR)/test_sm4.o, $(OBJECTS))
	@echo "静态库 libsm4.a 创建完成"

# 共享库
shared: $(OBJECTS)
	$(CC) -shared -fPIC -o libsm4.so $(filter-out $(SRCDIR)/test_sm4.o, $(OBJECTS))
	@echo "共享库 libsm4.so 创建完成"

# 文档生成
docs:
	@echo "生成文档..."
	@if command -v doxygen >/dev/null 2>&1; then \
		doxygen Doxyfile; \
		echo "文档生成完成，请查看 docs/html/index.html"; \
	else \
		echo "Doxygen未安装，请运行: sudo apt-get install doxygen"; \
		echo "或者跳过文档生成"; \
	fi

# 代码格式化
format:
	@echo "格式化代码..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRCDIR) $(INCDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		echo "代码格式化完成"; \
	else \
		echo "clang-format未安装，请运行: sudo apt-get install clang-format"; \
		echo "或者跳过代码格式化"; \
	fi

# 代码检查
lint:
	@echo "代码检查..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all $(SRCDIR) $(INCDIR); \
		echo "代码检查完成"; \
	else \
		echo "cppcheck未安装，请运行: sudo apt-get install cppcheck"; \
		echo "或者跳过代码检查"; \
	fi

# 清理
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f $(EXAMPLE_TARGETS)
	rm -f $(TEST_TARGETS)
	rm -f libsm4.a libsm4.so
	rm -rf build/*
	rm -rf docs/html docs/latex

# 完全清理
distclean: clean
	rm -rf build dist

# 打包
package: clean
	@echo "创建发布包..."
	@mkdir -p dist
	tar -czf dist/sm4-project-$(shell date +%Y%m%d).tar.gz \
		--exclude='*.o' --exclude='sm4_test' --exclude='build' \
		--exclude='.git' --exclude='*.tar.gz' .

# 帮助
help:
	@echo "可用的目标:"
	@echo "  all          - 编译主程序"
	@echo "  examples     - 编译示例程序"
	@echo "  tests        - 编译测试程序"
	@echo "  test         - 运行主测试"
	@echo "  test-all     - 运行所有测试"
	@echo "  perf         - 运行性能测试"
	@echo "  check_cpu    - 检查CPU特性"
	@echo "  setup        - 创建项目目录结构"
	@echo "  lib          - 创建静态库"
	@echo "  shared       - 创建共享库"
	@echo "  docs         - 生成文档"
	@echo "  format       - 格式化代码"
	@echo "  lint         - 代码检查"
	@echo "  clean        - 清理编译文件"
	@echo "  distclean    - 完全清理"
	@echo "  package      - 创建发布包"
	@echo "  install_deps - 安装依赖"
	@echo "  install_dev_tools - 安装开发工具 (doxygen, clang-format, cppcheck)"

.PHONY: all test test-all perf check_cpu clean distclean install_deps install_dev_tools help setup examples tests lib shared docs format lint package 