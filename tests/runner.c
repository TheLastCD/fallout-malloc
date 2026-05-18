#include <stdio.h>

int run_test_alloc(void);

int main(void)
{
    int failures = 0;

    printf("Running tests...\n");

    if (run_test_alloc() != 0) {
        failures++;
        fprintf(stderr, "test_alloc failed\n");
    }

    if (failures == 0) {
        printf("All tests passed\n");
        return 0;
    }

    printf("%d test(s) failed\n", failures);
    return 1;
}
