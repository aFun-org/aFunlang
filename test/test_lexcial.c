#include <stdio.h>
#include "aFun.h"

char *str = "Hello_var\n"
            "10 20.32 100var\n"
            "|10 20.32|int->num\n"
            "|10||20.32|int->num\n"
            "{if true 10}\n"
            "of(HelloWorld)\n"
            "!(!) @(@) #(#)\n"
            "() [] {}\n"
            "Hello ;comment\n"
            "Hello ;;comment\n"
            "commment2;=\n"
            "var-200 ;; comment\n"
            ";; comment\n"
            ";= comment\n"
            ";= var-300\n";

int main() {
    af_Parser *parser = makeParserByString(str, false, stderr);
    af_TokenType tt;
    char *text = NULL;

    do {
        tt = getTokenFromLexical(&text, parser);
        printf("tt = %d, text = %s\n", tt, text);
        free(text);
    } while (tt != TK_EOF && tt != TK_ERROR);

    freeParser(parser);
    return 0;
}