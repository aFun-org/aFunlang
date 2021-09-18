/*
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
 * 函数名: wstrWithWchar
 * 目标: 赋值str到新空间，并且扩展 size 个大小, 若write为true则为扩展到大小写入...字符
 */
wchar_t *wstrWithWchar(wchar_t *str, size_t size, int free_old, ...) {  // free_base使用int而不是bool, 是因为va_start
    size_t base_len = WSTR_LEN(str);
    wchar_t *tmp = NEW_WSTR(base_len + size);
    if (base_len != 0)
        wcscpy(tmp, str);
    va_list va;
    va_start(va, free_old);
    for (int i = 0; i < size; i++)
        tmp[base_len + i] = (wchar_t)va_arg(va, int);
    va_end(va);
    if (free_old)
        free(str);
    return tmp;
}

/*
 * 函数名: wstrWithWchar_
 * 目标: 在str后增加一个新的字符
 */
wchar_t *wstrWithWchar_(wchar_t *str, wint_t new, bool free_old) {
    size_t base_len = WSTR_LEN(str);
    wchar_t *tmp = NEW_WSTR(base_len + 1);
    if (base_len != 0)
        wcscpy(tmp, str);
    tmp[base_len] = new;
    if (free_old)
        free(str);
    return tmp;
}

/*
 * 函数名: wstrExpansion
 * 目标: 把str复制到新的空间, 并拓展其大小
 */
wchar_t *wstrExpansion(wchar_t *str, size_t size, bool free_old) {
    size_t base_len = WSTR_LEN(str);
    wchar_t *tmp = NEW_WSTR(base_len + size);
    if (base_len != 0)
        wcscpy(tmp, str);
    if (free_old)
        free(str);
    return tmp;
}

/*
 * 函数名: strJoinIter
 * 目标: 在base后面拼接...字符串，...必须以NULL结尾
 */
char *strJoinIter(char *base, int free_base, ...) {  // free_base使用int而不是bool, 是因为va_start
    va_list ap;
    va_start(ap, free_base);
    for (char *ch = va_arg(ap, char *); ch != NULL; ch = va_arg(ap, char *)) {
        base = strJoin(base, ch, free_base, false);
        free_base = true;
    }
    va_end(ap);
    return base;
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