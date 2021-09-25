#include <stdlib.h>
#include <stdio.h>
#include "aFun.h"
#include "../../src/core/__code.h"

int main() {
    af_Code *bt1 = makeElementCode("data", ',', 0, "Unknown");
    af_Code *bt2 = makeElementCode("var1", 0, 1, NULL);

    af_Code *bt3 = makeElementCode("data2", 0, 0, NULL);
    af_Code *bt4 = makeElementCode("var2", 0, 1, NULL);

    pushCode(&bt1, bt2);
    pushCode(&bt3, bt4);

    af_Code *bt5 = makeBlockCode(parentheses, bt3, 0, 1, NULL, NULL);
    pushCode(&bt2, bt5);

    if(!writeAllCode(bt1, "test.afb")) {
        fprintf(stderr, "Write test.afb error.\n");
        return EXIT_FAILURE;
    }

    af_Code *get;
    if(!readAllCode(&get, "test.afb")) {
        fprintf(stderr, "Read test.afb error.\n");
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

