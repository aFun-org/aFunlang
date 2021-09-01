#ifndef AFUN_REGEX_H
#define AFUN_REGEX_H
#include "regex.h"

struct af_Regex {
    pcre2_code *re;  // 正则表达式
    char *pattern;  // 正则表达式的字符串
};

#endif //AFUN_REGEX_H
