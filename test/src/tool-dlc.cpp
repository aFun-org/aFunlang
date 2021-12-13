#include <cstdio>
#include <cstdlib>
#include "tool.hpp"
using namespace aFuntool;

int test_func() {
    return 100;
}

int main(int argc, char **argv) {
    atexit(dlcExit);

    if (argc != 2)
        return EXIT_FAILURE;

    char *lib = argv[1];
    DlcHandle *dlc = openLibrary(lib, RTLD_NOW);  // TEST_LIB_PATH 传进来的分隔符 都是 "/"
    if (dlc == nullptr) {
        fprintf(stderr, "libary not found!\n");
        return EXIT_FAILURE;
    }

    typedef int (func)(int a);
    typedef int (test)();

    DlcSymbol<int> *a;
    DlcSymbol<func> *fun;
    DlcSymbol<test> *test_fun;

    a = dlc->get_symbol<int>("num");
    fun = dlc->get_symbol<func>("test");
    test_fun = new DlcSymbol<test>(test_func, nullptr);

    int test_func_result = test_fun->getSymbol()();

    printf("a = %d, test = %d\n", *(a->getSymbol()), fun->getSymbol()(test_func_result));

    delete a;
    delete fun;
    delete test_fun;
    dlc->close();
    return EXIT_SUCCESS;
}