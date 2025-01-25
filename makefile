CC=gcc
CFLAGS=-Wall -g -Iinc -fexec-charset=GBK
TARGET=cc
BUILD=build
BIN=bin

# src files
SRCS+=$(wildcard src/*.c)
SRCS+=$(wildcard src/syntax/*.c)
# obj files
OBJS=$(addprefix $(BUILD)/, $(SRCS:%.c=%.o))

all:$(BIN)/${TARGET}

$(BIN)/${TARGET}: $(OBJS)
	@mkdir -p "$(BIN)"
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/%.o:%.c
	@mkdir -p "$(dir $@)"
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -r "$(BUILD)"
	rm -r "$(BIN)"
