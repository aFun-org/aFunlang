#ifndef AFUN_HASH_H
#define AFUN_HASH_H
#include "aFunToolExport.h"

/* 哈希工具 */
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    typedef long int time33_t;
    AFUN_TOOL_C_EXPORT_FUNC time33_t time33(const char *str);
#ifdef __cplusplus
    AFUN_TOOL_EXPORT time33_t time33(const std::string &str);
#endif

#ifndef AFUN_TOOL_C
}
#endif

#endif //AFUN_HASH_H
