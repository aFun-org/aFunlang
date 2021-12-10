#ifndef AFUN_REGEX
#define AFUN_REGEX

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#include "aFunToolExport.h"

namespace aFuntool {
    const int REGEX_ERROR_SIZE = 512;

    class RegexException : public std::exception
    {
        std::string message = "Regex Error";
    public:
        explicit RegexException(std::string &msg) {
            this->message = "RegexErrpr: " + msg;
        }

        explicit RegexException(const char *msg) {
            this->message = std::string("RegexErrpr: ") + msg;
        }

        virtual const char *what() {
            return message.c_str();
        }
    };

    struct af_Regex {
        pcre2_code *re;  // 正则表达式
        char *pattern;  // 正则表达式的字符串
    };

    class Regex {
        pcre2_code *re;  // 正则表达式
        std::string pattern;  // 正则表达式的字符串
    public:
        explicit Regex(const std::string &pattern);
        ~Regex();
        int match(const char *subject);
    };
}

#endif //AFUN_REGEX
