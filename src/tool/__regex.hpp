#ifndef TOOL_REGEX_H_
#define TOOL_REGEX_H_
#include "regex.hpp"

struct af_Regex {
    pcre2_code *re;  // 正则表达式
    char *pattern;  // 正则表达式的字符串
};

#endif //TOOL_REGEX_H_
