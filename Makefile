# 定义变量
CXX := /usr/bin/clang++
CXXFLAGS := -fcolor-diagnostics -fansi-escape-codes -std=c++17 -g
LDFLAGS := -lcurses
SRC_DIR := src
OUT_DIR := out
TARGET := $(OUT_DIR)/main
DATA_FILE := data/1

# 查找所有子目录中的 .cpp 文件
SRCS := $(wildcard $(SRC_DIR)/*/*.cpp)

# 将 .cpp 文件的路径替换为 .o 文件路径，存储在 out 文件夹中
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OUT_DIR)/%.o, $(SRCS))

# 默认目标
all: $(TARGET) copy_data

# 链接目标
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# 编译每个 .cpp 文件，并将 .o 文件输出到 out 目录
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 复制 data/123.txt 文件到 out 目录
copy_data: $(DATA_FILE)
	mkdir -p $(OUT_DIR)
	cp $(DATA_FILE) $(OUT_DIR)/

# 清理目标
clean:
	rm -rf $(OUT_DIR)

# 伪目标
.PHONY: all clean copy_data
