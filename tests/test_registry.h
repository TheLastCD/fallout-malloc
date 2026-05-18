#ifndef TEST_REGISTRY_H
#define TEST_REGISTRY_H

typedef int (*TestFunc)(void);

struct TestCase {
    const char *name;
    TestFunc func;
    struct TestCase *next;
};

void register_test(const char *name, TestFunc func);

#define REGISTER_TEST(fn) \
    static void __attribute__((constructor)) register_##fn(void) { \
        register_test(#fn, fn); \
    }

#endif // TEST_REGISTRY_H
