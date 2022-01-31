#ifndef AFUN_TOOL_REGEX_INLINE_H
#define AFUN_TOOL_REGEX_INLINE_H
#include "tool-regex.h"

namespace aFuntool {
    inline Regex::Regex(const Regex &regex) noexcept: Regex(regex.pattern) {

    }

    inline Regex::Regex(Regex &&regex) noexcept : pattern {std::move(regex.pattern)}, re {regex.re} {
        regex.re = nullptr;
    }

    Regex::~Regex() noexcept {
        if (re != nullptr)
            pcre2_code_free(re);
    }
}

#endif //AFUN_TOOL_REGEX_INLINE_H
