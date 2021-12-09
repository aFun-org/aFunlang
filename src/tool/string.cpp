﻿/*
 * 文件名: string.c
 * 目标: 关于char和wchar_t的实用函数
 */

#include <cstdlib>
#include <cstring>
#include "tool.hpp"

char *charToStr(char ch) {
    if (ch == NUL)
        return nullptr;
    char *tmp = NEW_STR(1);
    *tmp = ch;
    return tmp;
}

char *strCopy(const char *str){
    char *tmp = NEW_STR(STR_LEN(str));
    if (str != nullptr)
        strcpy(tmp, str);
    return tmp;
}


/*
 * 函数名: strJoin
 * 目标: 拼接两个字符串
 */
char *strJoin(const char *first, const char *second, bool free_first, bool free_last) {
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
 * 函数名: strJoin_
 * 目标: 拼接两个字符串, 比 strJoin 更快
 */
char *strJoin_(const char *first, const char *second, bool free_first, bool free_last) {
    char *new_str = NEW_STR(STR_LEN(first) + STR_LEN(second));
    strcat(new_str, first);
    if (second != nullptr)
        strcat(new_str, second);

    if (free_first) {
        auto free_ = const_cast<char *>(first);
        free(free_);
    }

    if (free_last) {
        auto free_ = const_cast<char *>(first);
        free(free_);
    }
    return new_str;
}

/*
 * 函数名: convertToWstr
 * 目标: char *转换为wchar_t *
 */
wchar_t *convertToWstr(const char *str, bool free_old) {
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
char *convertToStr(const wchar_t *wstr, bool free_old) {
    size_t len = WSTR_LEN(wstr);
    auto tmp = NEW_STR(len);
    wcstombs(tmp, wstr, len);

    if (free_old) {
        auto free_ = const_cast<wchar_t *>(wstr);
        free(free_);
    }
    return tmp;
}