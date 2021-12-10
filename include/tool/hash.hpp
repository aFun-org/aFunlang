#ifndef AFUN_HASH_HPP
#define AFUN_HASH_HPP
#include "aFunToolExport.h"

/* 哈希工具 */
namespace aFuntool {
    typedef long int time33_t;
    AFUN_TOOL_EXPORT time33_t time33(const char *str);
    AFUN_TOOL_EXPORT time33_t time33(const std::string &str);
}

#endif //AFUN_HASH_HPP
