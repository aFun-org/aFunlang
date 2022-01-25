#ifndef AFUN_TOOL_REGEX
#define AFUN_TOOL_REGEX
#include <regex>

namespace aFuntool {
    class Regex {  // 整个对象都是inline的, 不需要Export符号
        std::regex re;  // 正则表达式
        std::string pattern;  // 正则表达式的字符串
    public:
        inline explicit Regex(std::string pattern_);
        inline Regex(const Regex &regex) noexcept;
        inline Regex(Regex &&regex) noexcept;
        Regex &operator=(const Regex &regex)=delete;
        Regex &operator=(Regex &&regex)=delete;

        [[nodiscard]] inline bool match(const char *subject) const;
        [[nodiscard]] inline bool match(const std::string &subject) const;
    };
}

#include "tool-regex.inline.h"

#endif //AFUN_TOOL_REGEX
