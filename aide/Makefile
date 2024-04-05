CC = gcc
LEX = flex
YACC = bison

# 指定头文件搜索路径
INCLUDES = -I include

# 指定需要链接的库
LIBS = -lgcrypt -lgelf -lmhash -lpcre -lz -lcap -lxattr

# 指定编译器标志
CFLAGS = -Wall $(INCLUDES)
LFLAGS = $(LIBS)

# 源文件目录和对象文件目录
SRC_DIR = src
OBJ_DIR = obj

# 自动生成源文件和对象文件列表
SRC = $(wildcard $(SRC_DIR)/*.c) $(SRC_DIR)/conf_lex.c $(SRC_DIR)/conf_yacc.c $(SRC_DIR)/db_lex.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# 指定最终目标
TARGET = myproject

# 默认目标
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

# 规则生成对象文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# 特殊处理yacc和lex文件
$(SRC_DIR)/conf_yacc.c: $(SRC_DIR)/conf_yacc.y
	$(YACC) -o $@ $<

$(SRC_DIR)/conf_lex.c: $(SRC_DIR)/conf_lex.l
	$(LEX) -o $@ $<

$(SRC_DIR)/db_lex.c: $(SRC_DIR)/db_lex.l
	$(LEX) -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

# 清理目标
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(SRC_DIR)/conf_lex.c $(SRC_DIR)/conf_yacc.c $(SRC_DIR)/db_lex.c

.PHONY: all clean
