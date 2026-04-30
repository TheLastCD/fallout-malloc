#ifdef ENABLE_ALLOC_DUMP

#include "memspace.h"
#include <stdio.h>

#define MAX_ALLOCATIONS 128

static int allocation_count = 0;

static struct Allocation {
    int offset;
    int size;
} allocations[MAX_ALLOCATIONS];

void record_allocation(int size, int *ptr)
{
    if (allocation_count >= MAX_ALLOCATIONS || ptr == NULL) {
        return;
    }

    allocations[allocation_count].offset = ptr - mem;
    allocations[allocation_count].size = size;
    allocation_count++;
}

void reset_allocations(void)
{
    allocation_count = 0;
}

void dump_allocations(void)
{
    printf("{\"allocations\":[");
    for (int i = 0; i < allocation_count; i++) {
        printf("{\"offset\":%d,\"size\":%d}", allocations[i].offset, allocations[i].size);
        if (i + 1 < allocation_count) {
            printf(",");
        }
    }
    printf("],\"top\":%d,\"mem_total\":%d}\n", top, mem_total);
}

#endif
