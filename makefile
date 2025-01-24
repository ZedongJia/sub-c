CC=gcc
CFLAGS=-Wall -g -Iinc -fexec-charset=GBK
TARGET=cc
BUILD=build
BIN=bin

# src files
SRCS+=$(wildcard src/*.c)
SRCS+=$(wildcard src/syntax/*.c)
# obj files
OBJS=$(SRCS:%.c=%.o)

all:$(BIN)/${TARGET}

$(BIN)/${TARGET}: $(OBJS)
	@mkdir -p "$(BIN)"
	@$(CC) $(CFLAGS) $(addprefix $(BUILD)/, $^) -o $@

$(OBJS):%.o:%.c
	@mkdir -p "$(dir $(BUILD)/$@)"
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $(BUILD)/$@ 

clean:
	rm -r "$(BUILD)"
	rm -r "$(BIN)"
