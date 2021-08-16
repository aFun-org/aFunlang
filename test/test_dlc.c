#include <stdio.h>
#include <stdlib.h>
#include "tool.h"

int main() {
    atexit(dlcExit);

    DlcHandle *dlc = openLibary(TEST_LIB_PATH "/libTest1" SHARED_MARK, RTLD_NOW);  // TEST_LIB_PATH 传进来的分隔符 都是 "/"
    if (dlc == NULL) {
        fprintf(stderr, "libary not found!\n");
        exit(EXIT_FAILURE);
    }

    typedef int func(int a);
    NEW_DLC_SYMBOL(int, INT);
    NEW_DLC_SYMBOL(func, FUNC);

    DLC_SYMBOL(INT) *a;
    DLC_SYMBOL(FUNC) *fun;

    a = READ_SYMBOL(dlc, "num", INT);
    fun = READ_SYMBOL(dlc, "test", FUNC);

    printf("a = %d, test = %d\n", GET_SYMBOL(a), GET_SYMBOL(fun)(GET_SYMBOL(a)));

    FREE_SYMBOL(a);
    FREE_SYMBOL(fun);

    if (!freeLibary(dlc))
        exit(EXIT_FAILURE);
    return 0;
}