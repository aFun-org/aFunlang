#include <cstdio>
#include "tool.hpp"

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"
#include "__regex.hpp"

af_Regex *makeRegex(const char *pattern, char **error) {
    if (error)
        *error = nullptr;
    if (!isCharUTF8(pattern)) {
        *error = strCopy("Pattern not utf-8");
        return nullptr;
    }

    int error_code;
    size_t erroroffset;
    char regex_error[REGEX_ERROR_SIZE];
    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &error_code, &erroroffset, nullptr);

    if (re == nullptr) {
        if (error) {
            PCRE2_UCHAR buffer[256];
            pcre2_get_error_message(error_code, buffer, sizeof(buffer));

            snprintf(regex_error, sizeof(regex_error), "Regex failed: %d: %s\n", (int) erroroffset, buffer);
            *error = strCopy(regex_error);
        }
        return nullptr;
    }

    auto rg = calloc(1, af_Regex);
    rg->pattern = strCopy(pattern);
    rg->re = re;
    return rg;
}

void freeRegex(af_Regex *rg) {
    pcre2_code_free(rg->re);
    free(rg->pattern);
    free(rg);
}

/*
 * 函数名: matchRegex
 * 目标: 检查一个字符串是否可被完全匹配一个正则表达式
 * 返回  (1) - 可完全匹配
 * 返回  (0) - 不可完全匹配或不可匹配
 * 返回 (>0) - 失败
 */
int matchRegex(const char *subject, af_Regex *rg, char **error) {
    if (error != nullptr)
        *error = nullptr;
    if (!isCharUTF8(subject)) {
        if (error != nullptr)
            *error = strCopy("Subject not utf-8");
        return 0;
    }

    char regex_error[REGEX_ERROR_SIZE];
    auto sub = (PCRE2_SPTR)subject;
    PCRE2_SIZE sub_len = strlen(subject);
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(rg->re, nullptr);
    int rc = pcre2_match(rg->re, sub, sub_len, 0, 0, match_data, nullptr);

    if (rc < 0) {
        pcre2_match_data_free(match_data);
        if (rc == PCRE2_ERROR_NOMATCH)
            return 0;
        else {
            if (error != nullptr) {
                snprintf(regex_error, sizeof(regex_error), "Regex match '%s' failed by '%s'\n", subject, rg->pattern);
                *error = strCopy(regex_error);
            }
            return -1;
        }
    }

    PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
    if (ovector[0] > ovector[1]) {
        if (error != nullptr) {
            snprintf(regex_error, sizeof(regex_error),
                     "\\K was used in an assertion to set the match start after its end.\n"
                     "From end to start the match was: %.*s\n",
                     (int) (ovector[0] - ovector[1]), (char *) (subject + ovector[1]));
            *error = strCopy(regex_error);
        }
        pcre2_match_data_free(match_data);
        return -2;
    }

    int result = 0;
    if (ovector[0] == 0 && ovector[1] == sub_len) // 完全匹配
        result = 1;
    pcre2_match_data_free(match_data);
    return result;
}
