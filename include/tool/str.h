#ifndef AFUN_STR_H
#define AFUN_STR_H
#include "aFunToolExport.h"

#ifdef __cplusplus
#include <cwchar>
#include <cstring>
#else
#include <wchar.h>
#include <string.h>
#endif

/* 字符串工具 */

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_TOOL_C_EXPORT_FUNC char *charToStr(char ch);
    AFUN_TOOL_C_EXPORT_FUNC char *strCopy(const char *str);
    AFUN_TOOL_C_EXPORT_FUNC char *strJoin(const char *first, const char *second, bool free_first, bool free_last);

#ifndef AFUN_TOOL_C
}
#endif

#endif //AFUN_STR_H
