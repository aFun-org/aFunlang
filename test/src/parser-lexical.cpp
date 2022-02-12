#include <cstdio>
#include "parser-parser.h"

const char *str = "Hello_var\n"
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

class ReaderString : public aFunparser::Reader {
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
    auto reader = ReaderString(str, "str");
    auto parser = aFunparser::Parser(reader);

    aFunparser::Parser::TokenType tt;
    std::string text;

    do {
        tt = parser.getTokenFromLexical(text);
        printf("tt = %d, text = %s\n", tt, text.c_str());
    } while (tt != aFunparser::Parser::TK_EOF && tt != aFunparser::Parser::TK_ERROR);

    return 0;
}