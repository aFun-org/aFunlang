#include <stdio.h>
#include "aFun.h"

char *str = "10 '20.32 100var\n"
            "|10||20.32|int->num\n"
            "{if true 10}\n"
            "of(HelloWorld)\n"
            "[Hello]\n"
            ",[Hello]\n"
;

int main() {
    af_Parser *parser = makeParserByString(str, false, stderr);
    af_Code *code = parserCode(parser);
    printCode(code);
    freeParser(parser);
    freeAllCode(code);
    return 0;
}