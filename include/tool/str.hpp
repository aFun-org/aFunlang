#ifndef AFUN_STR_HPP
#define AFUN_STR_HPP
#include <cwchar>
#include <cstring>
#include "aFunToolExport.h"

/* 字符串工具 */
#define EQ_STR(str1, str2) (!strcmp((str1), (str2)))
#define EQ_WSTR(wid1, wid2) (!wcscmp((wid1), (wid2)))

#define NEW_STR(size) calloc((size) + 1, char)
#define NEW_WSTR(size) calloc((size) + 1, wchar_t)

#define STR_LEN(p) (((p) == NULL) ? 0 : strlen((p)))
#define WSTR_LEN(p) (((p) == NULL) ? 0 : wcslen((p)))

AFUN_TOOL_EXPORT char *charToStr(char ch);
AFUN_TOOL_EXPORT char *strCopy(const char *str);
AFUN_TOOL_EXPORT char *strJoin(const char *first, const char *second, bool free_first, bool free_last);
AFUN_TOOL_EXPORT char *strJoin_(const char *first, const char *second, bool free_first, bool free_last);
AFUN_TOOL_EXPORT wchar_t *convertToWstr(const char *str, bool free_old);
AFUN_TOOL_EXPORT char *convertToStr(const wchar_t *wstr, bool free_old);

#endif //AFUN_STR_HPP
