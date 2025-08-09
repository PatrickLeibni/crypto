CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -I./include
LDFLAGS = -lm

# Source files organized by category
COREDIR = core
ATTACKSDIR = attacks
MERKLEDIR = merkle
UTILSDIR = utils
SRCDIR = src

# Core SM3 implementations
CORE_SOURCES = $(wildcard $(COREDIR)/*.c)
CORE_OBJECTS = $(CORE_SOURCES:.c=.o)

# Attack implementations
ATTACK_SOURCES = $(wildcard $(ATTACKSDIR)/*.c)
ATTACK_OBJECTS = $(ATTACK_SOURCES:.c=.o)

# Merkle tree implementation
MERKLE_SOURCES = $(wildcard $(MERKLEDIR)/*.c)
MERKLE_OBJECTS = $(MERKLE_SOURCES:.c=.o)

# Utility functions
UTILS_SOURCES = $(wildcard $(UTILSDIR)/*.c)
UTILS_OBJECTS = $(UTILS_SOURCES:.c=.o)

# Main source
MAIN_SOURCES = $(wildcard $(SRCDIR)/*.c)
MAIN_OBJECTS = $(MAIN_SOURCES:.c=.o)

# All objects
OBJECTS = $(CORE_OBJECTS) $(ATTACK_OBJECTS) $(MERKLE_OBJECTS) $(UTILS_OBJECTS) $(MAIN_OBJECTS)

# Test files
TESTDIR = tests
TEST_SOURCES = $(wildcard $(TESTDIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_TARGETS = $(TEST_SOURCES:.c=)

# Example files
EXAMPLEDIR = examples
EXAMPLE_SOURCES = $(wildcard $(EXAMPLEDIR)/*.c)
EXAMPLE_OBJECTS = $(EXAMPLE_SOURCES:.c=.o)
EXAMPLE_TARGETS = $(EXAMPLE_SOURCES:.c=)

# Target executable
TARGET = sm3_test

# Default target
all: $(TARGET) tests examples

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Build test executables
tests: $(TEST_TARGETS)

$(TESTDIR)/%: $(TESTDIR)/%.o $(filter-out src/main.o, $(OBJECTS))
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Test build complete: $@"

# Build example executables
examples: $(EXAMPLE_TARGETS)

$(EXAMPLEDIR)/%: $(EXAMPLEDIR)/%.o $(filter-out src/main.o, $(OBJECTS))
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Example build complete: $@"

# Compile source files from different directories
$(COREDIR)/%.o: $(COREDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ATTACKSDIR)/%.o: $(ATTACKSDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MERKLEDIR)/%.o: $(MERKLEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(UTILSDIR)/%.o: $(UTILSDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files
$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile example files
$(EXAMPLEDIR)/%.o: $(EXAMPLEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(COREDIR)/*.o $(ATTACKSDIR)/*.o $(MERKLEDIR)/*.o $(UTILSDIR)/*.o $(SRCDIR)/*.o $(TESTDIR)/*.o $(EXAMPLEDIR)/*.o $(TARGET) $(TEST_TARGETS) $(EXAMPLE_TARGETS)
	@echo "Clean complete"

# Run all tests in order: basic -> length-extension -> merkle
test-all: $(TARGET) tests
	@echo "=========================================="
	@echo "开始完整测试流程"
	@echo "=========================================="
	@echo ""
	@echo "1. 基本SM3功能测试"
	@echo "=========================================="
	./$(TESTDIR)/test_sm3_basic
	@echo ""
	@echo "2. 长度扩展攻击测试"
	@echo "=========================================="
	./$(TESTDIR)/test_length_extension
	@echo ""
	@echo "3. Merkle树测试 (10万叶子节点)"
	@echo "=========================================="
	./$(TESTDIR)/test_merkle_tree
	@echo ""
	@echo "=========================================="
	@echo "所有测试完成！"
	@echo "=========================================="

# Run individual test programs
test-basic: $(TESTDIR)/test_sm3_basic
	./$(TESTDIR)/test_sm3_basic

test-length-extension: $(TESTDIR)/test_length_extension
	./$(TESTDIR)/test_length_extension

test-merkle: $(TESTDIR)/test_merkle_tree
	./$(TESTDIR)/test_merkle_tree

test-performance: $(TESTDIR)/test_performance
	./$(TESTDIR)/test_performance

# Run example programs
run-examples: examples
	@echo "Running example programs..."
	@for example in $(EXAMPLE_TARGETS); do \
		echo "Running $$example..."; \
		./$$example; \
		echo ""; \
	done

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build the main executable, tests, and examples"
	@echo "  tests            - Build test executables"
	@echo "  examples         - Build example executables"
	@echo "  clean            - Remove build artifacts"
	@echo "  test-all         - Run all tests (basic -> length-extension -> merkle)"
	@echo "  test-basic       - Run basic SM3 tests"
	@echo "  test-length-extension - Run length extension attack tests"
	@echo "  test-merkle      - Run Merkle tree tests"
	@echo "  test-performance - Run performance tests"
	@echo "  run-examples     - Run all example programs"
	@echo "  help             - Show this help"

# Phony targets
.PHONY: all clean test-all test-basic test-length-extension test-merkle test-performance run-examples help tests examples 