#include <cstdio>
#include <cstdlib>
#include "aFuntool.h"
using namespace aFuntool;

int test_func() {
    return 100;
}

int main(int argc, char **argv) {
    atexit(DlcHandle::dlcExit);

    if (argc != 2)
        return EXIT_FAILURE;

    char *lib = argv[1];
    DlcHandle dlc {lib, RTLD_NOW};  // TEST_LIB_PATH 传进来的分隔符 都是 "/"
    if (dlc.isOpen()) {
        fprintf(stderr, "libary not found!\n");
        return EXIT_FAILURE;
    }

    typedef int (func)(int a);
    typedef int (test)();

    DlcSymbol<int> a = dlc.getSymbol<int>("num");
    DlcSymbol<func> fun = dlc.getSymbol<func>("test");
    DlcSymbol<test> test_fun{test_func, nullptr};

    int test_func_result = test_fun.getSymbol()();

    printf("a = %d, test = %d\n", *(a.getSymbol()), fun.getSymbol()(test_func_result));

    dlc.close();
    return EXIT_SUCCESS;
}