#ifndef AFUN_STR_H
#define AFUN_STR_H
#include <wchar.h>
#include <string.h>
#include "aFunToolExport.h"

/* 字符串工具 */
#define EQ_STR(str1, str2) (!strcmp((str1), (str2)))
#define EQ_WSTR(wid1, wid2) (!wcscmp((wid1), (wid2)))

#define pathCopy(path) ((FilePath)strCopy((char *)(path)))

#define NEW_STR(size) (char *)calloc((size) + 1, sizeof(char))
#define NEW_WSTR(size) (wchar_t *)calloc((size) + 1, sizeof(wchar_t))
#define STR_LEN(p) (((p) == NULL) ? 0 : strlen((p)))
#define WSTR_LEN(p) (((p) == NULL) ? 0 : wcslen((p)))
AFUN_TOOL_EXPORT char *charToStr(char ch);
AFUN_TOOL_EXPORT char *strCopy(const char *str);
AFUN_TOOL_EXPORT wchar_t *wstrCopy(const wchar_t *str);
AFUN_TOOL_EXPORT wchar_t *wstrWithWchar(wchar_t *str, size_t size, int free_old, ...);
AFUN_TOOL_EXPORT wchar_t *wstrWithWchar_(wchar_t *str, wint_t new, bool free_old);
AFUN_TOOL_EXPORT wchar_t *wstrExpansion(wchar_t *str, size_t size, bool free_old);
AFUN_TOOL_EXPORT char *strJoinIter(char *base, int free_base, ...);
AFUN_TOOL_EXPORT char *strJoin(char *first, char *second, bool free_first, bool free_last);
AFUN_TOOL_EXPORT char *strJoin_(char *first, char *second, bool free_first, bool free_last);
AFUN_TOOL_EXPORT wchar_t *wstrJoin(wchar_t *first, wchar_t *second, bool free_first, bool free_last);
AFUN_TOOL_EXPORT wchar_t *wstrJoin_(wchar_t *first, wchar_t *second, bool free_first, bool free_last);
AFUN_TOOL_EXPORT wchar_t *wstrCopySelf(wchar_t *str, long times);
AFUN_TOOL_EXPORT wchar_t *wstrReverse(wchar_t *str);
AFUN_TOOL_EXPORT wchar_t *convertToWstr(char *str, bool free_old);
AFUN_TOOL_EXPORT char *convertToStr(wchar_t *wstr, bool free_old);

#endif //AFUN_STR_H
