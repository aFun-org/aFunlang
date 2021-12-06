#include <cstdlib>
#include <cstdio>
#include "aFun.hpp"
#include "../../src/core/__code.hpp"

int main() {
    af_Code *bt1 = makeElementCode("data", '$', 0, "Unknown");
    af_Code *bt2 = makeElementCode("var1", 0, 1, nullptr);

    af_Code *bt3 = makeElementCode("data2", 0, 0, nullptr);
    af_Code *bt4 = makeElementCode("var2", 0, 1, nullptr);

    pushCode(&bt1, bt2);
    pushCode(&bt3, bt4);

    af_Code *bt5 = makeBlockCode(parentheses, bt3, 0, 1, nullptr, nullptr);
    pushCode(&bt2, bt5);

    int re;
    if((re = writeByteCode(bt1, "test.aub")) != 1) {
        fprintf(stderr, "Write test.aub error[%d].\n", re);
        return EXIT_FAILURE;
    }

    af_Code *get = nullptr;
    if((re = readByteCode(&get, "test.aub")) != 1) {
        fprintf(stderr, "Read test.aub error[%d].\n", re);
        return EXIT_FAILURE;
    }

    printf("out:\n");
    printCode(bt1);
    printf("in:\n");
    printCode(get);
    freeAllCode(bt1);
    freeAllCode(get);
    return EXIT_SUCCESS;
}

