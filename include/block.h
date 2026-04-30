
#ifndef BLOCK_H
#define BLOCK_H
typedef struct{
	int size;
	int handle;
	int locked;
	int* handler_ptr;
}Block;



Block init_block(int size);

int make_memory_request(Block *blk);



int del_block();
#endif

