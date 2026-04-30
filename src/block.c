#include "block.h"
#include "memspace.h"
#include <stddef.h>

Block init_block(int size){
	Block data;
	data.size = size;
	data.locked = 0;
	data.handle = 0;
	data.handler_ptr = NULL;
	return data;
}

int make_memory_request(Block *blk)
{
	blk->handler_ptr = request_memory(blk->size);
	return 0;
}

