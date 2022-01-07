#ifndef AFUN_MD5_H
#define AFUN_MD5_H
#include "aFunToolExport.h"

/* md5计算工具 */

namespace aFuntool {
    const int READ_DATA_SIZE = 1024;
    const int MD5_SIZE = 16;
    const int MD5_STR_LEN = MD5_SIZE * 2;
    const int MD5_STRING = MD5_STR_LEN + 1;

    typedef struct MD5_CTX MD5_CTX;

    AFUN_TOOL_EXPORT MD5_CTX *MD5Init();
    AFUN_TOOL_EXPORT void MD5Final(MD5_CTX *context, unsigned char digest[16]);
    AFUN_TOOL_EXPORT void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int input_len);

    template <typename T>
    T getFileMd5 (T &path);
}

#endif //AFUN_MD5_H
