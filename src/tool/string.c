﻿/*
 * 文件名: string.c
 * 目标: 关于char和wchar_t的实用函数
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "tool.h"

char *charToStr(char ch) {
    if (ch == NUL)
        return NULL;
    char *tmp = NEW_STR(1);
    *tmp = ch;
    return tmp;
}

char *strCopy(const char *str){
    char *tmp = NEW_STR(STR_LEN(str));
    if (str != NULL)
        strcpy(tmp, str);
    return tmp;
}

wchar_t *wstrCopy(const wchar_t *str){
    wchar_t *tmp = NEW_WSTR(WSTR_LEN(str));
    if (str != NULL)
        wcscpy(tmp, str);
    return tmp;
}

/*
 * 函数名: strJoin
 * 目标: 拼接两个字符串
 */
char *strJoin(char *first, char *second, bool free_first, bool free_last) {
    if (first == NULL && second == NULL)
        return NULL;
    else if (first == NULL){
        first = second;
        second = NULL;
        free_first = free_last;
        free_last = false;
    }

    char *new = NEW_STR(STR_LEN(first) + STR_LEN(second));
    strcat(new, first);
    if (second != NULL)
        strcat(new, second);

    if (free_first)
        free(first);
    if (free_last)
        free(second);
    return new;
}

/*
 * 函数名: strJoin_
 * 目标: 拼接两个字符串, 比 strJoin 更快
 */
char *strJoin_(char *first, char *second, bool free_first, bool free_last) {
    char *new = NEW_STR(STR_LEN(first) + STR_LEN(second));
    strcat(new, first);
    if (second != NULL)
        strcat(new, second);
    if (free_first)
        free(first);
    if (free_last)
        free(second);
    return new;
}

/*
 * 函数名: wstrJoin
 * 目标: 拼接两个宽字符串
 */
wchar_t *wstrJoin(wchar_t *first, wchar_t *second, bool free_first, bool free_last) {
    if (first == NULL && second == NULL)
        return NULL;
    else if (first == NULL){
        first = second;
        second = NULL;
        free_first = free_last;
        free_last = false;
    }

    wchar_t *new = wstrExpansion(first, WSTR_LEN(second), false);
    if (second != NULL)
        wcscat(new, second);

    if (free_first)
        free(first);
    if (free_last)
        free(second);
    return new;
}

/*
 * 函数名: wstrJoin_
 * 目标: 拼接两个宽字符串, 比 wstrJoin 更快
 */
wchar_t *wstrJoin_(wchar_t *first, wchar_t *second, bool free_first, bool free_last) {
    wchar_t *new = wstrExpansion(first, WSTR_LEN(second), false);
    if (second != NULL)
        wcscat(new, second);

    if (free_first)
        free(first);
    if (free_last)
        free(second);
    return new;
}

/*
 * 函数名: wstrCopySelf
 * 目标: 自我赋值 times 次, 若times小于0则会反转字符串
 */
wchar_t *wstrCopySelf(wchar_t *str, long times){
    bool need_free = false;
    wchar_t *new_str = NULL;
    if (times < 0){
        str = wstrReverse(str);
        times = -times;
        need_free = true;
    }
    for (long i=0; i < times; i++)
        new_str = wstrJoin_(new_str, str, true, false);
    if (need_free)
        free(str);
    return new_str;
}

/*
 * 函数名: wstrReverse
 * 目标: 反转字符串
 */
wchar_t *wstrReverse(wchar_t *str){
    size_t len_str = WSTR_LEN(str);
    wchar_t *new_str = NEW_WSTR(len_str);
    for (int i = 0;i < len_str;i++)
        new_str[i] = str[len_str - i - 1];
    return new_str;
}

/*
 * 函数名: convertToWstr
 * 目标: char *转换为wchar_t *
 */
wchar_t *convertToWstr(char *str, bool free_old) {
    size_t len = STR_LEN(str);
    wchar_t *tmp = NEW_WSTR(len);
    mbstowcs(tmp, str, len);
    if (free_old)
        free(str);
    return tmp;
}

/*
 * 函数名: convertToStr
 * 目标: wchar_t *转换为char *
 */
char *convertToStr(wchar_t *wstr, bool free_old) {
    size_t len = WSTR_LEN(wstr);
    char *tmp = NEW_STR(len);
    wcstombs(tmp, wstr, len);
    if (free_old)
        free(wstr);
    return tmp;
}