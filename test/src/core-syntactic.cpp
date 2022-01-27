#include <cstdio>
#include "core-parser.h"

const char *str = "{if true [HelloWorld (10)]}\n";
const char *str2 = "{if true [HelloWorld (10)\n";

class ReaderString : public aFuncore::Reader {
    std::string str;
    size_t index;
    size_t len;
public:
    ReaderString(std::string str_, const aFuntool::FilePath &path_) : Reader{path_, 0}, str{std::move(str_)} {
        index = 0;
        len = str.size();
    }

    size_t readText(char *dest, size_t read_len, ReadMode &mode) override {
        if (index == len)  // 读取到末尾
            return 0;

        if (index + read_len > len) {  // 超出长度范围
            read_len = len - index;
            mode = read_mode_finished;
        }

        memcpy(dest, str.c_str() + index, read_len);
        index += read_len;
        return read_len;
    }
};

int main() {
    {
        auto reader = ReaderString(str, "str");
        auto parser = aFuncore::Parser(reader);
        auto code = aFuncore::Code("test.aun");
        bool ret = parser.parserCode(code);
        if (!ret)
            return 1;
        code.display();
    }

    {
        auto reader = ReaderString(str2, "str2");
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