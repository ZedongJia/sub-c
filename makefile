CC=gcc
BIN=cc
SRCS=$(wildcard src/*.c)
OBJS=$(addprefix build/, $(SRCS:%.c=%.o))
CFLAGS=-g -Iinc -fexec-charset=GBK


build/${BIN}: $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@

build/%.o: %.c
	@mkdir -p "$(dir $@)"
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -r "build"
