#ifndef AFUN_TOOL_REGEX
#define AFUN_TOOL_REGEX
#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"
#include "macro.h"

namespace aFuntool {
    class AFUN_TOOL_EXPORT Regex {  // 整个对象都是inline的, 不需要Export符号
        std::string pattern;  // 正则表达式的字符串
        pcre2_code *re;  // 正则表达式
    public:
        explicit Regex(std::string pattern_) noexcept(false);
        AFUN_INLINE Regex(const Regex &regex) noexcept;
        AFUN_INLINE Regex(Regex &&regex) noexcept;
        AFUN_INLINE ~Regex() noexcept;
        Regex &operator=(const Regex &regex)=delete;
        Regex &operator=(Regex &&regex)=delete;

        [[nodiscard]] bool match(const std::string &subject) const;
    };
}

#include "tool-regex.inline.h"

#endif //AFUN_TOOL_REGEX
