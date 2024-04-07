CC = gcc
OFLAGS = -D_GNU_SOURCE -W -Wall -g -O2
CFLAGS = $(OFLAGS) -I./include -I./src
LDFLAGS = $(OFLAGS) -static -lm -lgcrypt -lpcre -lz -lgpg-error # -lgelf -lmhash -lcap -lxattr
FLEX = flex
YACC = bison -d

SRC_DIR = ./src
INC_DIR = ./include
OBJ_DIR = ./obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
DEPS = $(wildcard $(INC_DIR)/*.h)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

LEX_SRCS = $(wildcard $(SRC_DIR)/*.l)
YACC_SRCS = $(wildcard $(SRC_DIR)/*.y)
LEX_OBJS = $(LEX_SRCS:$(SRC_DIR)/%.l=$(OBJ_DIR)/%.o)
YACC_OBJS = $(YACC_SRCS:$(SRC_DIR)/%.y=$(OBJ_DIR)/%.o)

all: $(OBJ_DIR) aide

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(SRC_DIR)/conf_yacc.c: $(SRC_DIR)/conf_yacc.y
	$(YACC) -Wno-yacc -Wall -Werror -o $@ -p conf $<

$(SRC_DIR)/conf_lex.c: $(SRC_DIR)/conf_lex.l $(SRC_DIR)/conf_yacc.c
	$(LEX) -o$@ -Pconf $<

$(SRC_DIR)/db_lex.c: $(SRC_DIR)/db_lex.l $(SRC_DIR)/conf_yacc.h
	$(LEX) -o$@ -Pdb $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) 
	$(CC) -c $< -o $@ $(CFLAGS)

aide: $(YACC_OBJS) $(LEX_OBJS) $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)


.PHONY: clean
clean:
	rm -f $(SRC_DIR)/*_lex.c $(SRC_DIR)/*_yacc.c $(SRC_DIR)/*_yacc.h
	rm -f $(OBJ_DIR)/*.o aide
