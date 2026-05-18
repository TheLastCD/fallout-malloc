#include "memspace.h"
#include "block.h"
#include "test_registry.h"
#include <assert.h>
#include <stdio.h>

int run_test_alloc(void)
{
    assert(init_memspace(32) == 0);

    Block b = init_block(5);
    assert(make_memory_request(&b) == 0);
    assert(b.handler_ptr != NULL);

    for (int i = 0; i < b.size; i++) {
        b.handler_ptr[i] = i + 1;
    }

    for (int i = 0; i < b.size; i++) {
        assert(b.handler_ptr[i] == i + 1);
    }

    /* unregister/delete the block */
    assert(del_block(&b) == 0);

    printf("allocated and wrote %d ints via Block\n", 5);
    free_memspace();
    return 0;
}

REGISTER_TEST(run_test_alloc);
