#include "memspace.h"
#include "block.h"
#include "test_registry.h"
#include <assert.h>
#include <stdio.h>

int run_test_delete_squash(void)
{
    assert(init_memspace(100) == 0);

    Block a = init_block(5);
    Block b = init_block(3);
    Block c = init_block(4);

    make_memory_request(&a);
    make_memory_request(&b);
    make_memory_request(&c);

    assert(a.handler_ptr != NULL);
    assert(b.handler_ptr != NULL);
    assert(c.handler_ptr != NULL);

    /* write patterns */
    for (int i = 0; i < a.size; i++) a.handler_ptr[i] = 0xA1 + i;
    for (int i = 0; i < b.size; i++) b.handler_ptr[i] = 0xB1 + i;
    for (int i = 0; i < c.size; i++) c.handler_ptr[i] = 0xC1 + i;

    /* delete middle block */
    assert(del_block(&b) == 0);
    assert(b.handler_ptr == NULL);

    /* compact/move allocations */
    assert(squash() == 0);

    /* a and c should still contain their first element values */
    int found_a = 0, found_c = 0;
    if (a.handler_ptr) {
        assert(a.handler_ptr[0] == 0xA1);
        found_a = 1;
    }
    if (c.handler_ptr) {
        assert(c.handler_ptr[0] == 0xC1);
        found_c = 1;
    }

    printf("delete+squash: found_a=%d found_c=%d\n", found_a, found_c);

    free_memspace();
    return 0;
}

REGISTER_TEST(run_test_delete_squash);
