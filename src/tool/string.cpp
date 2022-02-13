/*
 * 文件名: string.c
 * 目标: 关于char和wchar_t的实用函数
 */

#include <cstdlib>
#include <cstring>
#include "tool.h"
#include "tool_str.h"

#define NEW_STR(size) safeCalloc<char>((size) + 1)
#define STR_LEN(p) (((p) == NULL) ? 0 : strlen((p)))

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    char *charToStr(char ch){
        if (ch == NUL)
            return nullptr;
        char *tmp = NEW_STR(1);
        *tmp = ch;
        return tmp;
    }

    char *strCopy(const char *str){
        if (str != nullptr) {
            auto size = STR_LEN(str);
            char *tmp = NEW_STR(size);
#ifdef AFUN_WIN32_NO_CYGWIN
            strcpy_s(tmp, size + 1, str);
#else
            strcpy(tmp, str);
#endif
            return tmp;
        }
        return nullptr;
    }
    
    /**
     * 拼接两个字符串
     */
    char *strJoin(const char *first, const char *second, bool free_first, bool free_last){
        if (first == nullptr && second == nullptr)
            return nullptr;
        else if (first == nullptr) {
            first = second;
            second = nullptr;
            free_first = free_last;
            free_last = false;
        }

        auto size = STR_LEN(first) + STR_LEN(second);
        char *new_str = NEW_STR(size);
#ifdef AFUN_WIN32_NO_CYGWIN
        strcat_s(new_str, size + 1, first);
#else
        strcat(new_str, first);
#endif
        if (second != nullptr) {
#ifdef AFUN_WIN32_NO_CYGWIN
            strcat_s(new_str, size + 1, second);
#else
            strcat(new_str, second);
#endif
        }

        if (free_first) {
            auto free_ = const_cast<char *>(first);
            safeFree(free_);
        }

        if (free_last) {
            auto free_ = const_cast<char *>(second);
            safeFree(free_);
        }
        return new_str;
    }

#ifndef AFUN_TOOL_C
}
#endif