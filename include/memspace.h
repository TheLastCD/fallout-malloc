
#define ALLOCATED_MEMORY 1028

#pragma once
#include "block.h"

extern int mem_total;
extern int top;
extern int *mem;

int init_memspace(int block_size);
int free_memspace();

int delete_memory(int handle);
int *request_memory(int size);
int *allocate_memory(int size);

#ifdef ENABLE_ALLOC_DUMP
void record_allocation(int size, int *ptr);
void reset_allocations(void);
void dump_allocations(void);
#else
static inline void record_allocation(int size, int *ptr)
{
    (void)size;
    (void)ptr;
}

static inline void reset_allocations(void)
{
}
#endif

int expand_memory(int toAllocate);
int shrink_memory(int toDeallocate);

int lock(int handle);
int unlock(int handle);

int squash();

/* Register a Block with the memspace so compaction can update pointers. */
int register_block(Block *blk);
int unregister_block(Block *blk);
/* Start a background TCP server (binds to localhost:port) that streams JSON layout updates.
 * Returns 0 on success. Call `stop_memspace_server()` to stop.
 */
#ifdef ENABLE_MEMSPACE_SERVER
int start_memspace_server(int port);
int stop_memspace_server(void);
#endif

