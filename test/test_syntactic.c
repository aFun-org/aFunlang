#include <stdio.h>
#include "aFun.h"

char *str = "10 '20.32 100var\n"
            "|10||20.32|int->num\n"
            "{if true 10}\n"
            "of(HelloWorld)\n"
            "[Hello]\n"
            ",[Hello]\n"
;

void test1(void);
void test2(void);

int main() {
    test1();
    test2();
    return 0;
}

void test1(void) {
    af_Parser *parser = makeParserByString(str, false, stderr);
    af_Code *code = parserCode(parser);
    printCode(code);
    freeParser(parser);
    freeAllCode(code);
}

void test2(void) {
    FilePath path = "./test.af";
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    if (fwrite(str, sizeof(char), strlen(str), file) != strlen(str)) {
        fprintf(stderr, "File write error.\n");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    af_Parser *parser = makeParserByFile(path, stderr);
    af_Code *code = parserCode(parser);
    printCode(code);
    freeParser(parser);
    freeAllCode(code);
}