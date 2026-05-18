#include "memspace.h"
#include "block.h"
#include <string.h>

int main(int argc, char *argv[])
{
    init_memspace(5120);

    Block data = init_block(32);
    make_memory_request(&data);

#ifdef ENABLE_ALLOC_DUMP
    if (argc > 1 && strcmp(argv[1], "--dump") == 0) {
        dump_allocations();
    }
#else
    (void)argc;
    (void)argv;
#endif

    return 0;
}
