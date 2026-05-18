CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
TEST_CFLAGS = -Itests

ifeq ($(SERVER),1)
CFLAGS += -pthread -DENABLE_MEMSPACE_SERVER
TEST_CFLAGS += -pthread -DENABLE_MEMSPACE_SERVER
endif
AR = ar
ARFLAGS = rcs

ifeq ($(DUMP),1)
CFLAGS += -DENABLE_ALLOC_DUMP
endif

LIB = libmemspace.a
OBJ = src/memspace.o src/memspace_diagnostics.o src/memstack.o src/block.o src/lock.o
MAIN = main
MAIN_SRC = main.c
TEST_DIR = tests
TEST = test_runner
TEST_SRC = $(TEST_DIR)/runner.c $(shell find $(TEST_DIR) -name 'test_*.c')

all: $(LIB) $(MAIN)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(MAIN): $(MAIN_SRC) $(LIB)
	$(CC) $(CFLAGS) $< $(LIB) -o $@

$(TEST): $(TEST_SRC) $(LIB)
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(TEST_SRC) $(LIB) -o $@

test: $(TEST)

demo: examples/demo.c
	$(CC) $(CFLAGS) -DENABLE_MEMSPACE_SERVER -Iinclude \
		examples/demo.c src/memspace.c src/memspace_diagnostics.c src/block.c src/lock.c src/memstack.c -o demo

dump:
	$(MAKE) DUMP=1 all

src/%.o: src/%.c include/memspace.h include/memspace.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lock.o: src/lock.c include/lock.h include/block.h
	$(CC) $(CFLAGS) -c $< -o $@

compdb:
	bear -- make clean all

clean:
	rm -f $(OBJ) $(LIB) $(MAIN) $(TEST) compile_commands.json

.PHONY: all clean compdb test dump
