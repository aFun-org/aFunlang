#ifndef AFUN_TOOL_ENCODING_H
#define AFUN_TOOL_ENCODING_H
#include "aFunToolExport.h"
#include "tool_macro.h"

#ifdef __cplusplus
#include <iostream>
#else
#include <stdio.h>
#endif

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_TOOL_C_EXPORT_FUNC bool isCharUTF8(const char *str);
#ifdef __cplusplus
    AFUN_INLINE bool isCharUTF8(const std::string &str);
#endif

#ifndef AFUN_TOOL_C
}
#endif

#include "tool_encoding.inline.h"

#endif //AFUN_TOOL_ENCODING_H
