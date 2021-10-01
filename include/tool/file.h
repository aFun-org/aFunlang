#ifndef AFUN_FILE_H
#define AFUN_FILE_H
#include "aFunToolExport.h"

/* 文件处理工具 */
AFUN_TOOL_EXPORT int checkFile(char *path);
AFUN_TOOL_EXPORT time_t getFileMTime(char *path);
AFUN_TOOL_EXPORT char *joinPath(char *path, char *name, char *suffix);
AFUN_TOOL_EXPORT char *getFileName(char *path_1);
AFUN_TOOL_EXPORT char *getFileNameWithPath(char *path_1);
AFUN_TOOL_EXPORT char *getFilePath(char *path_1, int dep);
AFUN_TOOL_EXPORT char *getFileSurfix(char *path);
AFUN_TOOL_EXPORT char *fileNameToVar(char *name, bool need_free);
AFUN_TOOL_EXPORT char *findPath(char *path, char *env, bool need_free);
AFUN_TOOL_EXPORT char *getExedir(char *pgm, int dep);
AFUN_TOOL_EXPORT uintmax_t getFileSize(char *path);
#endif //AFUN_FILE_H
