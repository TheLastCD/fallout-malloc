


typedef struct{
	int size;
	int handle;
	int locked;
	int* handler_ptr;
}Block;



Block init_block(int size);
int del_block();
