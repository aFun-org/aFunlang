﻿#ifndef AFUN_FILE_H
#define AFUN_FILE_H
#include "aFunToolExport.h"

/* 文件处理工具 */
AFUN_TOOL_EXPORT int checkFile(char *path);
AFUN_TOOL_EXPORT char *getFileName(char *path_1);
AFUN_TOOL_EXPORT char *fileNameToVar(char *name, bool need_free);
AFUN_TOOL_EXPORT char *findPath(char *path, char *env, bool need_free);

#endif //AFUN_FILE_H
