CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include 
LDADD += -lteec -L$(TEEC_EXPORT)/lib
#Add/link other required libraries here
CFLAGS += -D_GNU_SOURCE -W -g -O2 -I./include
LDADD += -D_GNU_SOURCE -W -Wall -g -O2 -static -lm -lgcrypt -lpcre -lz -lgpg-error -lconfig -lmhash# -lgelf  -lcap -lxattr

SRC_DIR = ./
INC_DIR = ./include

SRCS = $(wildcard $(SRC_DIR)/*.c)
DEPS = $(wildcard $(INC_DIR)/*.h)
OBJS = $(SRCS:$(SRC_DIR)/%.c=./%.o)


BINARY = optee_idtt

.PHONY: all
all: $(SRCS) $(DEPS) $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $< $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
