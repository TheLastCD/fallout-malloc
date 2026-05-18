#include "memspace.h"
#include "block.h"
#include "lock.h"
#include "test_registry.h"
#include <assert.h>
#include <stdio.h>

int run_test_request_invalid_size(void)
{
    assert(init_memspace(16) == 0);

    assert(request_memory(0) == NULL);
    assert(request_memory(-1) == NULL);
    assert(request_memory(17) == NULL);

    free_memspace();
    return 0;
}
REGISTER_TEST(run_test_request_invalid_size);

int run_test_zero_size_block(void)
{
    assert(init_memspace(16) == 0);

    Block b = init_block(0);
    assert(make_memory_request(&b) != 0 || b.handler_ptr == NULL);
    assert(del_block(&b) != 0);

    free_memspace();
    return 0;
}
REGISTER_TEST(run_test_zero_size_block);

int run_test_delete_unallocated(void)
{
    assert(init_memspace(32) == 0);

    Block b = init_block(4);
    assert(del_block(&b) == 1);

    free_memspace();
    return 0;
}
REGISTER_TEST(run_test_delete_unallocated);

int run_test_lock_unlock(void)
{
    assert(init_memspace(32) == 0);

    Block b = init_block(5);
    assert(make_memory_request(&b) == 0);
    assert(b.handler_ptr != NULL);

    assert(lock_block(&b) == LOCK_SUCCESS);
    assert(lock_block(&b) == LOCK_ERROR_ALREADY_LOCKED);
    assert(unlock_block(&b) == LOCK_SUCCESS);
    assert(unlock_block(&b) == LOCK_ERROR_NOT_LOCKED);

    assert(del_block(&b) == 0);
    free_memspace();
    return 0;
}
REGISTER_TEST(run_test_lock_unlock);

int run_test_squash_empty(void)
{
    assert(init_memspace(10) == 0);
    assert(squash() == 0);
    assert(top == 0);
    free_memspace();
    return 0;
}
REGISTER_TEST(run_test_squash_empty);

int run_test_squash_with_locked_block(void)
{
    assert(init_memspace(50) == 0);

    Block a = init_block(8);
    Block b = init_block(6);
    assert(make_memory_request(&a) == 0);
    assert(make_memory_request(&b) == 0);
    assert(lock_block(&a) == LOCK_SUCCESS);

    assert(del_block(&b) == 0);
    assert(squash() == 0);
    assert(a.handler_ptr != NULL);
    assert(a.handler_ptr == mem);
    assert(top == a.size);

    assert(unlock_block(&a) == LOCK_SUCCESS);
    assert(del_block(&a) == 0);
    free_memspace();
    return 0;
}
REGISTER_TEST(run_test_squash_with_locked_block);
