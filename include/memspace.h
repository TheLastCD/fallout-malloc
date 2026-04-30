
#define ALLOCATED_MEMORY 1028


#pragma once
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
