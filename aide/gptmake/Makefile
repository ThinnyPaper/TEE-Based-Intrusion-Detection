CC = gcc
CFLAGS = -Iinclude -Isrc -lgcrypt -lgelf -lmhash -lpcre -lz -lcap -lxattr
LEX = flex
YACC = bison
YFLAGS = -d # 命令bison生成头文件和源文件
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LEX_FILES = $(wildcard $(SRC_DIR)/*.l)
YACC_FILES = $(wildcard $(SRC_DIR)/*.y)
SRC_FILES = $(filter-out $(YACC_SRC), $(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
YACC_SRC = $(YACC_FILES:$(SRC_DIR)/%.y=$(SRC_DIR)/%.c)
YACC_OBJ = $(YACC_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LEX_SRC = $(LEX_FILES:$(SRC_DIR)/%.l=$(SRC_DIR)/%.c)
LEX_OBJ = $(LEX_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC = aide

all: $(OBJ_DIR) $(EXEC)

$(OBJ_DIR):
	mkdir -p $@

$(EXEC): $(YACC_OBJ) $(LEX_OBJ) $(OBJ_FILES)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(SRC_DIR)/%.c $(SRC_DIR)/%.h: $(SRC_DIR)/%.y
	$(YACC) $(YFLAGS) -o $(SRC_DIR)/$*.c $<

$(SRC_DIR)/%.c: $(SRC_DIR)/%.l
	$(LEX) -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(EXEC) $(YACC_SRC) $(LEX_SRC) $(YACC_SRC:%.c=%.h)
