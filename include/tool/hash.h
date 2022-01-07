#ifndef AFUN_HASH_H
#define AFUN_HASH_H
#include "aFunToolExport.h"

/* 哈希工具 */
namespace aFuntool {
    typedef long int time33_t;
    AFUN_TOOL_EXPORT time33_t time33(const char *str);
    AFUN_TOOL_EXPORT time33_t time33(const std::string &str);
}

#endif //AFUN_HASH_H
