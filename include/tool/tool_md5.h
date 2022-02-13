#ifndef AFUN_TOOL_MD5_H
#define AFUN_TOOL_MD5_H
#include "aFunToolExport.h"
#include "tool_macro.h"

/* md5计算工具 */

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_STATIC const size_t READ_DATA_SIZE = 1024;
    AFUN_STATIC const size_t MD5_SIZE = 16;
    AFUN_STATIC const size_t MD5_STR_LEN = 16 * 2;
    AFUN_STATIC const size_t MD5_STRING = (16 * 2) + 1;

    typedef struct MD5_CTX MD5_CTX;

    AFUN_TOOL_C_EXPORT_FUNC MD5_CTX *MD5Init();
    AFUN_TOOL_C_EXPORT_FUNC void MD5Final(MD5_CTX *context, unsigned char digest[16]);
    AFUN_TOOL_C_EXPORT_FUNC void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int input_len);

#ifdef __cplusplus
    template <typename T>
    T getFileMd5 (T &path) noexcept(false) ;
#endif

#ifndef AFUN_TOOL_C
}
#endif

#endif //AFUN_TOOL_MD5_H
