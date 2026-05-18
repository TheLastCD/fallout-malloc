#include "memspace.h"
#include "block.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(void)
{
    if (init_memspace(300) != 0) {
        fprintf(stderr, "failed to init memspace\n");
        return 1;
    }

#ifdef ENABLE_MEMSPACE_SERVER
    if (start_memspace_server(9999) != 0) {
        fprintf(stderr, "failed to start memspace server\n");
    } else {
        printf("memspace server listening on 127.0.0.1:9999\n");
    }
#else
    printf("Server not enabled in this build. Rebuild with SERVER=1.\n");
#endif

    /* simple allocate/free loop to demonstrate activity */
    for (int i = 0; i < 20; i++) {
        Block b = init_block(5 + (i % 10));
        make_memory_request(&b);
        if (b.handler_ptr) {
            for (int j = 0; j < b.size; j++) b.handler_ptr[j] = i + j;
        }

        /* every few iterations free an earlier allocation */
        if (i % 3 == 0) {
            del_block(&b);
        }

        if (i % 5 == 0) {
            squash();
        }

        struct timespec ts = {0, 200 * 1000 * 1000};
        nanosleep(&ts, NULL);
    }

#ifdef ENABLE_MEMSPACE_SERVER
    stop_memspace_server();
#endif

    free_memspace();
    return 0;
}
