/*
 * 文件名: string.c
 * 目标: 关于char和wchar_t的实用函数
 */

#include <cstdlib>
#include <cstring>
#include "macro.h"
#include "str.h"

using namespace aFuntool;

#define EQ_STR(str1, str2) (!strcmp((str1), (str2)))
#define EQ_WSTR(wid1, wid2) (!wcscmp((wid1), (wid2)))

#define NEW_STR(size) calloc((size) + 1, char)
#define NEW_WSTR(size) calloc((size) + 1, wchar_t)

#define STR_LEN(p) (((p) == NULL) ? 0 : strlen((p)))
#define WSTR_LEN(p) (((p) == NULL) ? 0 : wcslen((p)))

char *aFuntool::charToStr(char ch) {
    if (ch == NUL)
        return nullptr;
    char *tmp = NEW_STR(1);
    *tmp = ch;
    return tmp;
}

char *aFuntool::strCopy(const char *str){
    if (str != nullptr) {
        char *tmp = NEW_STR(STR_LEN(str));
        strcpy(tmp, str);
        return tmp;
    }
    return nullptr;
}


/*
 * 函数名: strJoin
 * 目标: 拼接两个字符串
 */
char *aFuntool::strJoin(const char *first, const char *second, bool free_first, bool free_last) {
    if (first == nullptr && second == nullptr)
        return nullptr;
    else if (first == nullptr){
        first = second;
        second = nullptr;
        free_first = free_last;
        free_last = false;
    }

    char *new_str = NEW_STR(STR_LEN(first) + STR_LEN(second));
    strcat(new_str, first);
    if (second != nullptr)
        strcat(new_str, second);

    if (free_first) {
        auto free_ = const_cast<char *>(first);
        free(free_);
    }

    if (free_last) {
        auto free_ = const_cast<char *>(second);
        free(free_);
    }
    return new_str;
}

/*
 * 函数名: convertToWstr
 * 目标: char *转换为wchar_t *
 */
wchar_t *aFuntool::convertToWstr(const char *str, bool free_old) {
    size_t len = STR_LEN(str);
    auto tmp = NEW_WSTR(len);
    mbstowcs(tmp, str, len);

    if (free_old) {
        auto free_ = const_cast<char *>(str);
        free(free_);
    }
    return tmp;
}

/*
 * 函数名: convertToStr
 * 目标: wchar_t *转换为char *
 */
char *aFuntool::convertToStr(const wchar_t *wstr, bool free_old) {
    size_t len = WSTR_LEN(wstr);
    auto tmp = NEW_STR(len);
    wcstombs(tmp, wstr, len);

    if (free_old) {
        auto free_ = const_cast<wchar_t *>(wstr);
        free(free_);
    }
    return tmp;
}