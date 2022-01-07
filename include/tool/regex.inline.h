#ifndef AFUN_REGEX_INLINE_H
#define AFUN_REGEX_INLINE_H

#include "regex.h"

namespace aFuntool {
    inline int Regex::match(const std::string &subject){
        return match(subject.c_str());
    }
}

#endif //AFUN_REGEX_INLINE_H
