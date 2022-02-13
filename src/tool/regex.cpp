#include "tool_exception.h"
#include "tool_regex.h"
#include "tool_encoding.h"
#include "string"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    Regex::Regex(std::string pattern_) noexcept(false): pattern{std::move(pattern_)}, re{nullptr} {
        int error_code;
        size_t error_offset;
        pcre2_code *ret = pcre2_compile((PCRE2_SPTR) pattern.c_str(), PCRE2_ZERO_TERMINATED, 0, &error_code,
                                        &error_offset, nullptr);

        if (ret == nullptr) {
            if (error_code) {
                PCRE2_UCHAR buffer[256];
                pcre2_get_error_message(error_code, buffer, sizeof(buffer));
                char regex_error[1024];
                snprintf(regex_error, sizeof(regex_error), "Regex failed: %d: %s\n", (int) error_offset, buffer);
                throw RegexException(regex_error);
            } else
                throw RegexException("Regex failed: unknown");
        }

        if (!isCharUTF8(pattern)) {
            pcre2_code_free(ret);
            throw RegexException("Pattern not utf-8");
        }

        re = ret;
    }

    bool Regex::match(const std::string &subject) const{
        if (!isCharUTF8(subject))
            throw RegexException("Subject not utf-8");

        auto sub = (PCRE2_SPTR)subject.c_str();
        PCRE2_SIZE sub_len = subject.size();
        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, nullptr);
        int rc = pcre2_match(re, sub, sub_len, 0, 0, match_data, nullptr);

        if (rc < 0) {
            pcre2_match_data_free(match_data);
            if (rc == PCRE2_ERROR_NOMATCH)
                return false;
            else {
                char regex_error[1024];
                snprintf(regex_error, sizeof(regex_error), "Regex match '%s' failed by '%s'\n", subject.c_str(), pattern.c_str());
                throw RegexException(regex_error);
            }
        }

        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
        if (ovector[0] > ovector[1]) {
            char regex_error[1024];
            snprintf(regex_error, sizeof(regex_error),
                     "\\K was used in an assertion to set the match start after its end.\n"
                     "From end to start the match was: %.*s\n",
                     (int) (ovector[0] - ovector[1]), (char *) (subject.c_str() + ovector[1]));
            pcre2_match_data_free(match_data);
            throw RegexException(regex_error);
        }

        bool result = false;
        if (ovector[0] == 0 && ovector[1] == sub_len) // 完全匹配
            result = true;
        pcre2_match_data_free(match_data);
        return result;
    }

#ifndef AFUN_TOOL_C
}
#endif