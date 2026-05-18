#include "memspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_MEMSPACE_SERVER
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#endif

int mem_total = 0;
int top = 0;
int *mem = NULL;

int init_memspace(int block_size)
{
    if (!block_size) {
        block_size = ALLOCATED_MEMORY;
    }

    mem = malloc(block_size * sizeof *mem);
    if (!mem) {
        return 1;
    }

    mem_total = block_size;
    top = 0;
    reset_allocations();
    return 0;
}

int free_memspace()
{
    free(mem);
    mem = NULL;
    top = 0;
    reset_allocations();
    return 0;
}

int *request_memory(int size)
{
    if (size <= 0 || size > mem_total - top) {
        return NULL;
    }

    int *memory_ptr = allocate_memory(size);
    record_allocation(size, memory_ptr);
    return memory_ptr;
}

int *allocate_memory(int size)
{
    int *memory_ptr = mem + top;
    top += size;
    return memory_ptr;
}

/* Simple registry of Blocks so squash() can update handler_ptrs when moving data. */
static Block **block_registry = NULL;
static int registry_count = 0;
static int registry_capacity = 0;
#ifdef ENABLE_MEMSPACE_SERVER
static pthread_mutex_t registry_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Server state */
static pthread_t server_thread;
static int server_running = 0;
static int listen_fd = -1;

/* Helper: build JSON layout string into buffer, returns length */
static int build_layout_json(char *buf, size_t bufsize)
{
    int used = 0;
    used += snprintf(buf + used, bufsize - used, "{\"mem_total\":%d,\"top\":%d,\"allocations\":[", mem_total, top);

    int first = 1;
    for (int i = 0; i < registry_count; i++) {
        Block *b = block_registry[i];
        if (!b || b->handler_ptr == NULL || b->size <= 0) continue;
        int offset = (int)(b->handler_ptr - mem);
        if (!first) used += snprintf(buf + used, bufsize - used, ",");
        used += snprintf(buf + used, bufsize - used, "{\"offset\":%d,\"size\":%d,\"locked\":%d}", offset, b->size, b->locked ? 1 : 0);
        first = 0;
        if ((size_t)used >= bufsize - 100) break; /* avoid overflow */
    }

    used += snprintf(buf + used, bufsize - used, "]}\n");
    return used;
}

static void *server_thread_fn(void *arg)
{
    int port = (intptr_t)arg;
    struct sockaddr_in addr;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) return NULL;

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof addr) != 0) {
        close(listen_fd);
        listen_fd = -1;
        return NULL;
    }

    if (listen(listen_fd, 1) != 0) {
        close(listen_fd);
        listen_fd = -1;
        return NULL;
    }

    while (server_running) {
        int client = accept(listen_fd, NULL, NULL);
        if (client < 0) break;

        /* For this client, send layout periodically until disconnect */
        char buf[8192];
        while (server_running) {
            pthread_mutex_lock(&registry_mutex);
            int len = build_layout_json(buf, sizeof buf);
            pthread_mutex_unlock(&registry_mutex);

            ssize_t w = write(client, buf, (size_t)len);
            if (w <= 0) break;
            /* sleep a bit */
            struct timespec ts = {0, 200 * 1000 * 1000}; /* 200ms */
            nanosleep(&ts, NULL);
        }

        close(client);
    }

    if (listen_fd >= 0) { close(listen_fd); listen_fd = -1; }
    return NULL;
}

int start_memspace_server(int port)
{
    if (server_running) return 1;
    server_running = 1;
    if (pthread_create(&server_thread, NULL, server_thread_fn, (void*)(intptr_t)port) != 0) {
        server_running = 0;
        return 1;
    }
    return 0;
}

int stop_memspace_server(void)
{
    if (!server_running) return 1;
    server_running = 0;
    if (listen_fd >= 0) shutdown(listen_fd, SHUT_RDWR);
    pthread_join(server_thread, NULL);
    return 0;
}
#endif

int register_block(Block *blk)
{
    if (blk == NULL) {
        return 1;
    }

    if (registry_count >= registry_capacity) {
        int new_cap = registry_capacity ? registry_capacity * 2 : 64;
        Block **new_reg = realloc(block_registry, new_cap * sizeof *new_reg);
        if (!new_reg) {
            return 1;
        }
        block_registry = new_reg;
        registry_capacity = new_cap;
    }

    block_registry[registry_count++] = blk;
    return 0;
}

/* Compact movable (unlocked) allocations to remove gaps.
 * Locked blocks are treated as immovable barriers and are preserved in place.
 */
int squash()
{
    if (!mem) {
        return 1;
    }

    if (registry_count == 0) {
        /* nothing to do */
        top = 0;
        return 0;
    }

    /* Build an array of registered, live blocks with valid handler_ptrs */
    Block **list = malloc(registry_count * sizeof *list);
    if (!list) return 1;
    int count = 0;
    for (int i = 0; i < registry_count; i++) {
        Block *b = block_registry[i];
        if (b && b->handler_ptr != NULL && b->size > 0) {
            list[count++] = b;
        }
    }

    if (count == 0) {
        free(list);
        top = 0;
        return 0;
    }

    /* Sort by current offset using standard comparator */
    int cmpfn(const void *pa, const void *pb)
    {
        const Block *a = *(const Block * const *)pa;
        const Block *b = *(const Block * const *)pb;
        int off_a = (int)(a->handler_ptr - mem);
        int off_b = (int)(b->handler_ptr - mem);
        return (off_a > off_b) - (off_a < off_b);
    }

    qsort(list, count, sizeof *list, cmpfn);

    int next_free = 0;
    for (int i = 0; i < count; i++) {
        Block *b = list[i];
        int offset = (int)(b->handler_ptr - mem);
        int bsize = b->size;

        if (b->locked) {
            /* immovable barrier: ensure next_free moves past it */
            int end = offset + bsize;
            if (end > next_free) next_free = end;
            continue;
        }

        if (offset == next_free) {
            next_free += bsize;
            continue;
        }

        /* Move the block down to next_free */
        memmove(mem + next_free, mem + offset, (size_t)bsize * sizeof *mem);
        b->handler_ptr = mem + next_free;
        next_free += bsize;
    }

    top = next_free;
    free(list);
    return 0;
}

int unregister_block(Block *blk)
{
    if (blk == NULL) return 1;

    int found = 0;
    for (int i = 0; i < registry_count; i++) {
        if (block_registry[i] == blk) {
            found = 1;
            /* shift left remaining entries */
            for (int j = i; j + 1 < registry_count; j++) {
                block_registry[j] = block_registry[j + 1];
            }
            registry_count--;
            break;
        }
    }

    /* If the block had allocated memory, clear it and trim top if it was at the end. */
    if (blk->handler_ptr != NULL) {
        int offset = (int)(blk->handler_ptr - mem);
        int end = offset + blk->size;
        /* clear memory (optional) */
        memset(mem + offset, 0, (size_t)blk->size * sizeof *mem);
        blk->handler_ptr = NULL;
        if (end == top) {
            /* shrink top to remove trailing free space */
            top = offset;
        }
    }

    return found ? 0 : 1;
}
