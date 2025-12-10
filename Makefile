# Top-level Makefile for Ada to MIPS Compiler
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRC_DIR = ada_parser/src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/ada_compiler

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/lexer.c \
          $(SRC_DIR)/parser.c \
          $(SRC_DIR)/ast.c \
          $(SRC_DIR)/semantic.c \
          $(SRC_DIR)/symbol_table.c \
          $(SRC_DIR)/mips_codegen.c \
          $(SRC_DIR)/register_alloc.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Compiler built successfully: $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) output.asm

test: $(TARGET)
	@echo "=== Testing basic example ==="
	$(TARGET) examples/test_basic.ada -o output.asm
	@echo ""
	@echo "=== Testing arithmetic example ==="
	$(TARGET) examples/test_arithmetic.ada -o output.asm
	@echo ""
	@echo "=== Testing control structures example ==="
	$(TARGET) examples/test_control.ada -o output.asm
	@echo ""
	@echo "All tests passed!"

.PHONY: all clean test
