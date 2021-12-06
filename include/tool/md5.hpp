#ifndef AFUN_MD5_HPP
#define AFUN_MD5_HPP
#include "aFunToolExport.h"

/* md5计算工具 */
#define READ_DATA_SIZE	(1024)
#define MD5_SIZE (16)
#define MD5_STR_LEN (MD5_SIZE * 2)  // md5str的长度
#define MD5_STRING (MD5_STR_LEN + 1)

typedef struct MD5_CTX MD5_CTX;

AFUN_TOOL_EXPORT MD5_CTX *MD5Init(void);
AFUN_TOOL_EXPORT void MD5Final(MD5_CTX *context, unsigned char digest[16]);
AFUN_TOOL_EXPORT void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int input_len);
AFUN_TOOL_EXPORT char *getFileMd5(char *path);

#endif //AFUN_MD5_HPP
