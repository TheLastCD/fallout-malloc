#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "test_registry.h"

static struct TestCase *test_head = NULL;

void register_test(const char *name, TestFunc func)
{
    struct TestCase *entry = malloc(sizeof *entry);
    if (!entry) return;
    entry->name = name;
    entry->func = func;
    entry->next = test_head;
    test_head = entry;
}

static void timestamp(char *buf, size_t len)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);
    snprintf(buf, len, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

static int run_and_report(const char *name, int (*test)(void))
{
    char ts1[32];
    timestamp(ts1, sizeof ts1);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int res = test();
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    char ts2[32];
    timestamp(ts2, sizeof ts2);

    if (res == 0) {
        printf("[%s -> %s] TEST %s: PASS (%.3fs)\n", ts1, ts2, name, elapsed);
    } else {
        printf("[%s -> %s] TEST %s: FAIL (%.3fs)\n", ts1, ts2, name, elapsed);
    }
    return res != 0;
}

int main(void)
{
    int failures = 0;
    int total = 0;

    printf("Running tests...\n");

    for (struct TestCase *t = test_head; t != NULL; t = t->next) {
        failures += run_and_report(t->name, t->func);
        total += 1;
    }

    if (failures == 0) {
        printf("All tests passed (%d tests)\n", total);
        return 0;
    }

    printf("%d test(s) failed\n", failures);
    return 1;
}
