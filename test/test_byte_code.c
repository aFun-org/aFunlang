#include <stdlib.h>
#include <stdio.h>
#include "aFun.h"

int main() {
    af_Code *bt1 = makeLiteralCode("data", "func", false, ',', 0, "Unknown");
    af_Code *bt2 = makeVariableCode("var1", 0, 1, NULL);

    af_Code *bt3 = makeLiteralCode("data2", "func", false, 0, 0, NULL);
    af_Code *bt4 = makeVariableCode("var2", 0, 1, NULL);

    connectCode(&bt1, bt2);
    connectCode(&bt3, bt4);

    af_Code *bt5 = makeBlockCode(parentheses, bt3, 0, 1, NULL, NULL);
    connectCode(&bt2, bt5);

    FILE *file = fopen("test.afb", "wb");
    if (file == NULL) {
        fprintf(stderr, "Can't not creat file: test.afb\n");
        return EXIT_FAILURE;
    }

    if(!writeAllCode(bt1, file)) {
        fprintf(stderr, "Write test.afb error.\n");
        return EXIT_FAILURE;
    }
    fclose(file);

    af_Code *get;
    file = fopen("test.afb", "rb");
    if (file == NULL) {
        fprintf(stderr, "Can't not read file: test.afb\n");
        return EXIT_FAILURE;
    }

    if(!readAllCode(&get, file)) {
        fprintf(stderr, "Read test.afb error.\n");
        return EXIT_FAILURE;
    }
    fclose(file);

    printf("out:\n");
    printCode(bt1);
    printf("in:\n");
    printCode(get);
    freeAllCode(bt1);
    freeAllCode(get);
    return EXIT_SUCCESS;
}

