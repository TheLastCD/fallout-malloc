#include <stddef.h>
#include "lock.h"

/*
Pseudo code for enforcing the lock inside a function:

function perform_operation_on_block(handle):
    block = find_block_by_handle(handle)
    if block is NULL:
        return ERROR_INVALID_HANDLE

    result = lock_block(block)
    if result != LOCK_SUCCESS:
        return result

    # critical section begins
    do_work_with(block)
    # critical section ends

    unlock_block(block)
    return SUCCESS
*/

int lock_block(Block *blk)
{
    if (blk == NULL) {
        return LOCK_ERROR_INVALID_BLOCK;
    }

    if (blk->locked) {
        return LOCK_ERROR_ALREADY_LOCKED;
    }

    blk->locked = 1;
    return LOCK_SUCCESS;
}

int unlock_block(Block *blk)
{
    if (blk == NULL) {
        return LOCK_ERROR_INVALID_BLOCK;
    }

    if (!blk->locked) {
        return LOCK_ERROR_NOT_LOCKED;
    }

    blk->locked = 0;
    return LOCK_SUCCESS;
}
