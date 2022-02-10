#ifndef AFUN_TOOL_REGEX_INLINE_H
#define AFUN_TOOL_REGEX_INLINE_H
#ifdef __cplusplus

#include "tool-regex.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    Regex::Regex(const Regex &regex) noexcept: Regex(regex.pattern) {

    }

    Regex::Regex(Regex &&regex) noexcept : pattern {std::move(regex.pattern)}, re {regex.re} {
        regex.re = nullptr;
    }

    Regex::~Regex() noexcept {
        if (re != nullptr)
            pcre2_code_free(re);
    }

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_TOOL_REGEX_INLINE_H
