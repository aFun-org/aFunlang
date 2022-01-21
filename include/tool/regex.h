#ifndef AFUN_REGEX
#define AFUN_REGEX

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

#include "aFunToolExport.h"

namespace aFuntool {
    const int REGEX_ERROR_SIZE = 512;

    class AFUN_TOOL_EXPORT Regex {
        pcre2_code *re;  // 正则表达式
        const std::string pattern;  // 正则表达式的字符串
    public:
        explicit Regex(std::string pattern_);
        Regex(const Regex &regex);
        inline Regex(Regex &&regex) noexcept;
        Regex &operator=(const Regex &regex)=delete;
        Regex &operator=(Regex &&regex)=delete;
        ~Regex();

        int match(const char *subject);
        inline int match(const std::string &subject);
    };
}

#include "regex.inline.h"

#endif //AFUN_REGEX
