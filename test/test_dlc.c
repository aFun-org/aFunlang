#include <stdio.h>
#include <stdlib.h>
#include "tool.h"

int test_func(void) {
    return 100;
}

int main() {
    atexit(dlcExit);

    DlcHandle *dlc = openLibary(LIB_TEST1, RTLD_NOW);  // TEST_LIB_PATH 传进来的分隔符 都是 "/"
    if (dlc == NULL) {
        fprintf(stderr, "libary not found!\n");
        exit(EXIT_FAILURE);
    }

    typedef int func(int a);
    typedef int test(void);
    NEW_DLC_SYMBOL(int, INT);
    NEW_DLC_SYMBOL(func, FUNC);
    NEW_DLC_SYMBOL(test, TEST);

    DLC_SYMBOL(INT) a;
    DLC_SYMBOL(FUNC) fun;
    DLC_SYMBOL(TEST) test_fun;

    a = READ_SYMBOL(dlc, "num", INT);
    fun = READ_SYMBOL(dlc, "test", FUNC);
    test_fun = MAKE_SYMBOL(test_func, TEST);

    int test_func_result = GET_SYMBOL(test_fun)();

    printf("a = %d, test = %d\n", GET_SYMBOL(a), GET_SYMBOL(fun)(test_func_result));

    FREE_SYMBOL(a);
    FREE_SYMBOL(fun);
    FREE_SYMBOL(test_fun);

    if (!freeLibary(dlc))
        exit(EXIT_FAILURE);
    return 0;
}