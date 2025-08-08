# 获取当前目录下所有的 .c 文件（自动添加新的 demo）
SRC := $(wildcard *.c)

# 生成对应的可执行文件（去掉 .c 后缀）
TARGETS := $(basename $(SRC))

# 编译器和参数
CC := gcc
CFLAGS = -Wall -g

# 默认目标：编译所有
all: $(TARGETS)

# 单独规则：每个 .c 编译成对应的可执行文件
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# 清理所有目标文件
clean:
	rm -f $(TARGETS)
