#include <cstdio>
#include "aFunrt.h"

const char *str = "{if true [HelloWorld (10)]}\n";
const char *str2 = "{if true [HelloWorld (10)\n";

class ConsoleReader : public aFuncore::Reader {
public:
    size_t STDIN_MAX_SIZE = 1024;
    explicit ConsoleReader(std::function<bool()> interrupt_, const aFuntool::FilePath &path_="console.aun");
    size_t readText(char *dest, size_t read_len, ReadMode &mode) override;
private:
    char *data;
    size_t index;
    size_t len;
    bool no_first;
    std::function<bool()> interrupt;  // 中断函数
};

inline ConsoleReader::ConsoleReader(std::function<bool()> interrupt_, const aFuntool::FilePath &path_)
    : Reader{path_, 0}, interrupt{std::move(interrupt_)} {
    data = nullptr;
    index = 0;
    len = 0;
    no_first = false;
}


size_t ConsoleReader::readText(char *dest, size_t read_len, aFuncore::Reader::ReadMode &mode) {
    if (index == len) {  // 读取内容
        if (aFuntool::clear_stdin()) {
            mode = read_mode_error;
            return 0;
        }

        if (no_first)
            aFuntool::cout << "\r.... ";
        else {
            aFuntool::clear_stdin();
            aFuntool::cout <<  "\r>>>> ";
        }

        fflush(stdout);
        no_first = true;
        aFuntool::safeFree(data);

        /* 在Linux平台, 只用当数据写入stdin缓冲行时checkStdin才true */
        /* 在Windows平台则是根据读取的最后一个字符是否为\n或者是否有按键按下来确定缓冲区是否有内容 */
        while (!aFuntool::checkStdin()) {  // 无内容则一直循环等到
            if (interrupt()) {  // 设置了中断函数, 并且该函数返回0
                std::cout << "Interrupt\n";
                mode = read_mode_error;
                return 0;
            }
        }

        int ch = aFuntool::fgetc_stdin();
        if (ch == '\n' || ch == EOF) {
            /* 读取结束 */
            mode = read_mode_finished;
            return 0;
        }

        aFuntool::fungetc_stdin(ch);

        /* 读取内容的长度不得少于STDIN_MAX_SZIE, 否则可能导致编码转换错误 */
        if (aFuntool::fgets_stdin(&data, (int)STDIN_MAX_SIZE) == 0) {
            mode = read_mode_error;
            return 0;
        }

        index = 0;
        len = std::strlen(data);
    }

    if (index + read_len > len)  // 超出长度范围
        read_len = len - index;
    memcpy(dest, data + index, read_len);
    index += read_len;
    return read_len;
}

bool nothing() {
    return false;
}

int main(int argc, char **) {
    std::string md5_1;
    std::string md5_2;

    {
        auto reader = aFunrt::StringReader(str, "str");
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
        auto reader = aFunrt::StringReader(str2, "str2");
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

        auto reader = aFunrt::FileReader("test.aun");
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

    if (argc == 1)
        return 0;

    {
        auto reader = ConsoleReader(nothing, "stdin.aun");
        auto parser = aFuncore::Parser(reader);
        auto code = aFuncore::Code("stdin.aun");
        bool ret = parser.parserCode(code);

        if (ret) {
            code.display();
        } else {
            while (parser.countEvent() != 0) {
                auto event = parser.popEvent();
                printf("Event %d, %d\n", event.type, event.line);
            }
        }
    }

    return 0;
}