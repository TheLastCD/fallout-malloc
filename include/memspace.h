#include <stdio.h>
#include <stdlib.h>


#define ALLOCATED_MEMORY 1028

static void *mem;

int init_memspace(int block_size);

int request_memory(int size);
int delete_memory(int handle);


int expand_memory(int toAllocate);
int shrink_memory(int toDeallocate);

int lock(int handle);
int unlock(int handle);

int squash();
