# Makefile for RISC-V Matrix Extension Demo
# Demonstrates build system integration for custom instruction extensions

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = 

# Directories
SRC_DIR = simulator
TEST_DIR = tests
TOOLS_DIR = tools
BUILD_DIR = build

# Source files
SIMULATOR_SRC = $(SRC_DIR)/matmul_simulator.c
TEST_SRC = $(TEST_DIR)/test_matmul.c

# Targets
SIMULATOR = $(BUILD_DIR)/matmul_simulator
TEST_RUNNER = $(BUILD_DIR)/test_runner

# Default target
all: $(BUILD_DIR) $(SIMULATOR) $(TEST_RUNNER)
	@echo "Build complete!"
	@echo "Run 'make demo' to see the matrix multiplication in action"

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build simulator
$(SIMULATOR): $(SIMULATOR_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Simulator built successfully"

# Build test runner
$(TEST_RUNNER): $(TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Test runner built successfully"

# Run demonstration
demo: $(SIMULATOR)
	@echo "=== Running RISC-V MATMUL Demo ==="
	./$(SIMULATOR)

# Run tests
test: $(TEST_RUNNER)
	@echo "=== Running Test Suite ==="
	./$(TEST_RUNNER)

# Generate SAIL to CGEN translation
translate:
	@echo "=== SAIL to CGEN Translation Demo ==="
	@echo "Input SAIL specification:"
	@head -20 specs/matmul_sail.spec
	@echo ""
	@echo "Generated CGEN description:"
	@head -20 cgen/matmul_cgen.scm

# Show instruction encoding
encoding:
	@echo "=== RISC-V MATMUL Instruction Encoding ==="
	@echo "Instruction: matmul rd, rs1, rs2"
	@echo "Format: R-type"
	@echo "Encoding: func7[31:25] | rs2[24:20] | rs1[19:15] | func3[14:12] | rd[11:7] | opcode[6:0]"
	@echo "Values:   0000001     | rs2       | rs1       | 111       | rd      | 0101011"
	@echo ""
	@echo "Example: matmul x1, x2, x3"
	@echo "Binary:  0000001 00011 00010 111 00001 0101011"
	@echo "Hex:     0x0062F85B"

# Documentation generation
docs:
	@echo "=== Generating Documentation ==="
	@echo "Project demonstrates SAIL to CGEN translation for RISC-V extensions"
	@echo "See README.md for complete documentation"

# Performance benchmark
benchmark: $(SIMULATOR)
	@echo "=== Performance Benchmark ==="
	@echo "Testing matrix multiplication performance..."
	time ./$(SIMULATOR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Install (placeholder for system integration)
install: all
	@echo "Installation would copy tools to system directories"
	@echo "This is a demo project - no actual installation performed"

# Uninstall (placeholder)
uninstall:
	@echo "Uninstallation would remove installed tools"
	@echo "This is a demo project - no actual uninstallation performed"

# Help target
help:
	@echo "RISC-V Matrix Extension Demo - Available targets:"
	@echo ""
	@echo "  all        - Build everything (default)"
	@echo "  demo       - Run the matrix multiplication demonstration"
	@echo "  test       - Run the test suite"
	@echo "  translate  - Show SAIL to CGEN translation example"
	@echo "  encoding   - Display instruction encoding details"
	@echo "  docs       - Generate documentation"
	@echo "  benchmark  - Run performance benchmark"
	@echo "  clean      - Remove build artifacts"
	@echo "  install    - Install tools (demo only)"
	@echo "  uninstall  - Remove installed tools (demo only)"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make all && make demo"

# Phony targets
.PHONY: all demo test translate encoding docs benchmark clean install uninstall help

# Dependencies
$(SIMULATOR): $(SRC_DIR)/matmul_simulator.c
$(TEST_RUNNER): $(TEST_DIR)/test_matmul.c

# Show build information
info:
	@echo "=== Build Configuration ==="
	@echo "CC:      $(CC)"
	@echo "CFLAGS:  $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "SRC_DIR: $(SRC_DIR)"
	@echo "BUILD_DIR: $(BUILD_DIR)"
