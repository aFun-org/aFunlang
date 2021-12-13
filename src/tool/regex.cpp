#include <cstdio>
#include "tool.hpp"

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"
#include "regex.hpp"
using namespace aFuntool;

aFuntool::Regex::Regex(const std::string &pattern_) : pattern {pattern_} {
    if (!isCharUTF8(pattern))
        throw RegexException("Pattern not utf-8");

    int error_code;
    size_t erroroffset;
    char regex_error[REGEX_ERROR_SIZE];

    this->re = pcre2_compile((PCRE2_SPTR)pattern.c_str(), PCRE2_ZERO_TERMINATED, 0, &error_code, &erroroffset, nullptr);
    if (re == nullptr) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(error_code, buffer, sizeof(buffer));
        snprintf(regex_error, sizeof(regex_error), "R%d: %s\n", (int) erroroffset, buffer);
        throw RegexException(regex_error);
    }
}

aFuntool::Regex::~Regex() {
    pcre2_code_free(re);
}

/*
 * 函数名: matchRegex
 * 目标: 检查一个字符串是否可被完全匹配一个正则表达式
 * 返回  (1) - 可完全匹配
 * 返回  (0) - 不可完全匹配或不可匹配
 * 返回 (>0) - 失败
 */
int aFuntool::Regex::match(const char *subject) {
    if (!isCharUTF8(subject))
        throw RegexException("Subject not utf-8");

    char regex_error[REGEX_ERROR_SIZE];
    PCRE2_SIZE sub_len = strlen(subject);
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, nullptr);
    int rc = pcre2_match(re, (PCRE2_SPTR)subject, sub_len, 0, 0, match_data, nullptr);

    if (rc < 0) {
        pcre2_match_data_free(match_data);
        if (rc == PCRE2_ERROR_NOMATCH)
            return 0;
        else {
            snprintf(regex_error, sizeof(regex_error),
                     "Regex match '%s' failed by '%s'\n", subject, pattern.c_str());
            throw RegexException(regex_error);
        }
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
    if (ovector[0] > ovector[1]) {
        snprintf(regex_error, sizeof(regex_error),
                 "\\K was used in an assertion to set the match start after its end.\n"
                 "From end to start the match was: %.*s\n",
                 (int) (ovector[0] - ovector[1]), (char *) (subject + ovector[1]));
        pcre2_match_data_free(match_data);
        throw RegexException(regex_error);
    }

    int ret = 0;
    if (ovector[0] == 0 && ovector[1] == sub_len) // 完全匹配
        ret = 1;
    pcre2_match_data_free(match_data);
    return ret;
}
