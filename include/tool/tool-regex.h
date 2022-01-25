#ifndef AFUN_REGEX
#define AFUN_REGEX
#include "aFunToolExport.h"
#include <regex>

namespace aFuntool {
    const int REGEX_ERROR_SIZE = 512;

    class AFUN_TOOL_EXPORT Regex {
        std::regex re;  // 正则表达式
        std::string pattern;  // 正则表达式的字符串
    public:
        inline explicit Regex(std::string pattern_);
        inline Regex(const Regex &regex);
        inline Regex(Regex &&regex) noexcept;
        Regex &operator=(const Regex &regex)=delete;
        Regex &operator=(Regex &&regex)=delete;

        [[nodiscard]] inline bool match(const char *subject) const;
        [[nodiscard]] inline bool match(const std::string &subject) const;
    };
}

#include "tool-regex.inline.h"

#endif //AFUN_REGEX
