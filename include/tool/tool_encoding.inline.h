#ifndef AFUN_TOOL_ENCODING_INLINE_H
#define AFUN_TOOL_ENCODING_INLINE_H
#ifdef __cplusplus

#include "tool_encoding.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    bool isCharUTF8(const std::string &str) {
        return isCharUTF8(str.c_str());
    }

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_TOOL_ENCODING_INLINE_H
