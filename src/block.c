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
	if (blk->handler_ptr != NULL) {
		register_block(blk);
	}
	return 0;
}

int del_block(Block *blk)
{
	if (blk == NULL) return 1;
	/* ask memspace to unregister and trim top if possible */
	int result = unregister_block(blk);
	blk->handler_ptr = NULL;
	blk->size = 0;
	blk->handle = 0;
	blk->locked = 0;
	return result;
}

