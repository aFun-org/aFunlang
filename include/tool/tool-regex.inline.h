#ifndef AFUN_TOOL_REGEX_INLINE_H
#define AFUN_TOOL_REGEX_INLINE_H

#include "tool-regex.h"

namespace aFuntool {
    inline Regex::Regex(std::string pattern_) noexcept(false) : re{pattern_}, pattern{std::move(pattern_)} {
        if (!isCharUTF8(pattern))
            throw RegexException("Pattern not utf-8");
    }

    inline Regex::Regex(const Regex &regex) noexcept: re{regex.pattern}, pattern{regex.pattern}{

    }

    inline Regex::Regex(Regex &&regex) noexcept : pattern {std::move(regex.pattern)}, re {std::move(regex.re)} {

    }

    inline bool Regex::match(const char *subject) const{
        return std::regex_match(subject, re);
    }

    inline bool Regex::match(const std::string &subject) const {
        return std::regex_match(subject, re);
    }
}

#endif //AFUN_TOOL_REGEX_INLINE_H
