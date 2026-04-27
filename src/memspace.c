#include "memspace.h"


int init_memspace(int block_size) 
{
    if (!block_size){
        block_size = ALLOCATED_MEMORY;
    }
    mem = malloc(block_size);
    if (!mem){
        return 1;
    }
    return 0;
}


int request_memory(int size)
{
    return 0;
}
