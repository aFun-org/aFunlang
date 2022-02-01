#include <cstdio>
#include "aFunrt.h"

const char *str = "{if true [HelloWorld (10)]}\n";
const char *str2 = "{if true [HelloWorld (10)\n";

int main() {
    std::string md5_1;
    std::string md5_2;

    {
        auto reader = aFunrt::ReaderString(str, "str");
        auto parser = aFuncore::Parser(reader);
        auto code = aFuncore::Code("test.aun");
        bool ret = parser.parserCode(code);
        if (!ret)
            return 1;
        code.display();
        md5_1 = code.getMD5_v1();
        aFuntool::cout << "Code1 md5: %s" << md5_1 << "\n";
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

    {
        auto file = aFuntool::fileOpen("test.aun", "w");
        if (file == nullptr) {
            printf("Cannot open file test.aun\n");
            return 1;
        }
        fprintf(file, "%s", str);
        aFuntool::fileClose(file);

        auto reader = aFunrt::ReaderFile("test.aun");
        auto parser = aFuncore::Parser(reader);
        auto code = aFuncore::Code("test.aun");
        bool ret = parser.parserCode(code);
        if (!ret)
            return 1;
        code.display();
        md5_2 = code.getMD5_v1();
        aFuntool::cout << "Code2 md5: %s" << md5_2 << "\n";
    }

    std::cout << "Is md5_1 == md5_2? " << (md5_1 == md5_2) << "\n";
    return 0;
}