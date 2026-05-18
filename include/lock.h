#ifndef LOCK_H
#define LOCK_H

#include "block.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOCK_SUCCESS 0
#define LOCK_ERROR_INVALID_BLOCK 1
#define LOCK_ERROR_ALREADY_LOCKED 2
#define LOCK_ERROR_NOT_LOCKED 3

int lock_block(Block *blk);
int unlock_block(Block *blk);

#ifdef __cplusplus
}
#endif

#endif // LOCK_H
