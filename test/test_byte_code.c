#include <stdlib.h>
#include <stdio.h>
#include "aFun.h"

int main() {
    af_ByteCode *bt1 = makeLiteralByteCode("data", "func", ',', 0, "Unknow");
    af_ByteCode *bt2 = makeVariableByteCode("var1", 0, 1, NULL);

    af_ByteCode *bt3 = makeLiteralByteCode("data2", "func", 0, 0, NULL);
    af_ByteCode *bt4 = makeVariableByteCode("var2", 0, 1, NULL);

    connectByteCode(&bt1, bt2);
    connectByteCode(&bt3, bt4);

    af_ByteCode *bt5 = makeBlockByteCode(parentheses, bt3, 0, 1, NULL, NULL);
    connectByteCode(&bt2, bt5);

    FILE *file = fopen("test.afb", "wb");
    if (file == NULL) {
        fprintf(stderr, "Can't not creat file: test.afb\n");
        return EXIT_FAILURE;
    }

    if(!writeAllByteCode(bt1, file)) {
        fprintf(stderr, "Write test.afb error.\n");
        return EXIT_FAILURE;
    }
    fclose(file);

    af_ByteCode *get;
    file = fopen("test.afb", "rb");
    if (file == NULL) {
        fprintf(stderr, "Can't not read file: test.afb\n");
        return EXIT_FAILURE;
    }

    if(!readAllByteCode(&get, file)) {
        fprintf(stderr, "Read test.afb error.\n");
        return EXIT_FAILURE;
    }
    fclose(file);

    printf("out:\n");
    printByteCode(bt1);
    printf("in:\n");
    printByteCode(get);
    freeAllByteCode(bt1);
    freeAllByteCode(get);
    return EXIT_SUCCESS;
}

