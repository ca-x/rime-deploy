# 检测操作系统类型
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    EXE_EXT := .exe
    RM := del /Q
    MKDIR := mkdir
else
    DETECTED_OS := $(shell uname -s)
    EXE_EXT :=
    RM := rm -rf
    MKDIR := mkdir -p
endif

# 编译器设置
CC ?= gcc
CFLAGS := -Wall -Wextra -O2
LDFLAGS :=

# 目录结构
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# 源文件
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET := rime-deploy$(EXE_EXT)

# 默认目标
.PHONY: all clean dirs

all: $(TARGET)

# 创建目录（使用 -p 并忽略错误）
$(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR):
	-$(MKDIR) "$@"

# 编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接规则
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# 清理规则
clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(TARGET)