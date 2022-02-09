#ifndef AFUN_ENCODING_INLINE_H
#define AFUN_ENCODING_INLINE_H
#include "encoding.h"

namespace aFuntool {
    bool isCharUTF8(const std::string &str) {
        return isCharUTF8(str.c_str());
    }
}

#endif //AFUN_ENCODING_INLINE_H
