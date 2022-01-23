#ifndef AFUN_REGEX_INLINE_H
#define AFUN_REGEX_INLINE_H

#include "regex.h"

namespace aFuntool {
    inline int Regex::match(const std::string &subject) const {
        return match(subject.c_str());
    }

    inline Regex::Regex(Regex &&regex) noexcept : pattern {std::move(regex.pattern)}, re {regex.re} {
        regex.re = nullptr;
    }
}

#endif //AFUN_REGEX_INLINE_H
