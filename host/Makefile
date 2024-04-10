CC = gcc
OFLAGS = -D_GNU_SOURCE -W -Wall -g -O2
CFLAGS = $(OFLAGS) -I./
LDFLAGS = $(OFLAGS) -static -lm -lgcrypt -lpcre -lz -lgpg-error -lconfig# -lgelf -lmhash -lcap -lxattr
FLEX = flex
YACC = bison -d

SRC_DIR = ./
INC_DIR = ./
OBJ_DIR = ./obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
DEPS = $(wildcard $(INC_DIR)/*.h)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(OBJ_DIR) idtt

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) 
	$(CC) -c $< -o $@ $(CFLAGS)

idtt: $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)


.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o idtt
