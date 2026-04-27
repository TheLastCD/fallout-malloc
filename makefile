CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
AR = ar
ARFLAGS = rcs

LIB = libmemspace.a
OBJ = src/memspace.o src/memstack.o
MAIN = main
MAIN_SRC = main.c

all: $(LIB) $(MAIN)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(MAIN): $(MAIN_SRC) $(LIB)
	$(CC) $(CFLAGS) $< $(LIB) -o $@

src/%.o: src/%.c include/memspace.h
	$(CC) $(CFLAGS) -c $< -o $@

compdb:
	bear -- make clean all

clean:
	rm -f $(OBJ) $(LIB) $(MAIN) compile_commands.json

.PHONY: all clean compdb
