#include <cstdio>
#include "aFunrt.h"

const char *str = "{if true [HelloWorld (10)]}\n";
const char *str2 = "{if true [HelloWorld (10)\n";

int main() {
    {
        auto reader = aFunrt::ReaderString(str, "str");
        auto parser = aFuncore::Parser(reader);
        auto code = aFuncore::Code("test.aun");
        bool ret = parser.parserCode(code);
        if (!ret)
            return 1;
        code.display();
    }

    {
        auto reader = aFunrt::ReaderString(str2, "str2");
        auto parser = aFuncore::Parser(reader);
        auto code = aFuncore::Code("test2.aun");
        parser.parserCode(code);

        while (parser.countEvent() != 0) {
            auto event = parser.popEvent();
            printf("Event %d, %d\n", event.type, event.line);
        }
    }
    return 0;
}