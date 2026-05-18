#include "memspace.h"
#include <assert.h>
#include <stdio.h>

int run_test_alloc(void)
{
    assert(init_memspace(32) == 0);

    int *block = request_memory(5);
    assert(block != NULL);

    for (int i = 0; i < 5; i++) {
        block[i] = i + 1;
    }

    for (int i = 0; i < 5; i++) {
        assert(block[i] == i + 1);
    }

    printf("allocated and wrote %d ints\n", 5);
    free_memspace();
    return 0;
}
