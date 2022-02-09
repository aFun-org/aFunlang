/*
 * 文件名: string.c
 * 目标: 关于char和wchar_t的实用函数
 */

#include <cstdlib>
#include <cstring>
#include "tool-type.h"
#include "str.h"

#define NEW_STR(size) safeCalloc<char>((size) + 1)
#define STR_LEN(p) (((p) == NULL) ? 0 : strlen((p)))

namespace aFuntool {
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
            strcpy_s(tmp, size + 1, str);
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
        strcat_s(new_str, size + 1, first);
        if (second != nullptr)
            strcat_s(new_str, size + 1, second);

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
}