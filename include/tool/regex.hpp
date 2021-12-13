#ifndef AFUN_REGEX
#define AFUN_REGEX

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#include "aFunToolExport.h"

namespace aFuntool {
    const int REGEX_ERROR_SIZE = 512;

    class Regex {
        pcre2_code *re;  // 正则表达式
        const std::string pattern;  // 正则表达式的字符串
    public:
        explicit Regex (const std::string &pattern_);
        ~Regex ();
        int match(const char *subject);
    };
}

#endif //AFUN_REGEX
