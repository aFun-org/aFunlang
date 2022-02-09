#ifndef AFUN_ENCODING_H
#define AFUN_ENCODING_H
#include <iostream>
#include "aFunToolExport.h"
#include "macro.h"

namespace aFuntool {
    AFUN_TOOL_EXPORT bool isCharUTF8(const char *str);
    AFUN_INLINE bool isCharUTF8(const std::string &str);
}

#include "encoding.inline.h"

#endif //AFUN_ENCODING_H
