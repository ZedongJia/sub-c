CC=gcc
CFLAGS=-Wall -g -fexec-charset=GBK
TARGET=cc
BUILD=build
BIN=bin

# src files
SRCS+=$(wildcard *.c)
# obj files
OBJS=$(addprefix $(BUILD)/, $(SRCS:%.c=%.o))

all:dirs $(BIN)/${TARGET}

dirs:
	@if test ! -d "$(BUILD)"; then mkdir "$(BUILD)"; fi
	@if test ! -d "$(BIN)"  ; then mkdir "$(BIN)"  ; fi

$(BIN)/${TARGET}: $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/%.o:%.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	@if test -d "$(BUILD)"; then rm -r "$(BUILD)"; fi
	@if test -d "$(BIN)"  ; then rm -r "$(BIN)"  ; fi
