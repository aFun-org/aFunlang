#ifndef AFUN_STR_H
#define AFUN_STR_H
#include <cwchar>
#include <cstring>
#include "aFunToolExport.h"

/* 字符串工具 */

namespace aFuntool {
    AFUN_TOOL_EXPORT char *charToStr(char ch);
    AFUN_TOOL_EXPORT char *strCopy(const char *str);
    AFUN_TOOL_EXPORT char *strJoin(const char *first, const char *second, bool free_first, bool free_last);
}

#endif //AFUN_STR_H
