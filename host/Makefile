CC      ?= $(CROSS_COMPILE)gcc -v
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

# Compiler flags
CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include 
CFLAGS += -I./include
CFLAGS += -I/home/TP/libconfig-out/include -I/home/TP/libgcrypt/include

# Linker flags
LDFLAGS += -L/home/TP/libconfig-out/lib -L/home/TP/libgcrypt-out/lib
LDADD += -lteec -L$(TEEC_EXPORT)/lib -lm -lgcrypt -lpcre -lz -lgpg-error -lconfig  -static

SRC_DIR = ./
INC_DIR = ./include

SRCS = $(wildcard $(SRC_DIR)/*.c)
DEPS = $(wildcard $(INC_DIR)/*.h)
OBJS = $(SRCS:$(SRC_DIR)/%.c=%.o)

BINARY = optee_idtt

.PHONY: all
all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $< $(LDADD)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)
