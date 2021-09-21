﻿#ifndef AFUN_MD5_H
#define AFUN_MD5_H
#include "aFunToolExport.h"

/* md5计算工具 */
#define READ_DATA_SIZE	(1024)
#define MD5_SIZE (16)
#define MD5_STR_LEN (MD5_SIZE * 2)  // md5str的长度
#define MD5_STRING (MD5_STR_LEN + 1)

AFUN_TOOL_EXPORT char *getFileMd5(const char *path);

#endif //AFUN_MD5_H