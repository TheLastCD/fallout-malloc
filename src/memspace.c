#include "memspace.h"
#include <stdio.h>
#include <stdlib.h>

int mem_total = 0;
int top = 0;
int *mem = NULL;

int init_memspace(int block_size)
{
    if (!block_size) {
        block_size = ALLOCATED_MEMORY;
    }

    mem = malloc(block_size * sizeof *mem);
    if (!mem) {
        return 1;
    }

    mem_total = block_size;
    top = 0;
    reset_allocations();
    return 0;
}

int free_memspace()
{
    free(mem);
    mem = NULL;
    top = 0;
    reset_allocations();
    return 0;
}

int *request_memory(int size)
{
    if (size <= 0 || size > mem_total - top) {
        return NULL;
    }

    int *memory_ptr = allocate_memory(size);
    record_allocation(size, memory_ptr);
    return memory_ptr;
}

int *allocate_memory(int size)
{
    int *memory_ptr = mem + top;
    top += size;
    return memory_ptr;
}
