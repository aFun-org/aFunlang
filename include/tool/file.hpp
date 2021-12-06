#ifndef AFUN_FILE_HPP
#define AFUN_FILE_HPP
#include "aFunToolExport.h"

/* 文件处理工具 */
AFUN_TOOL_EXPORT int checkFile(const char *path);
AFUN_TOOL_EXPORT time_t getFileMTime(const char *path);
AFUN_TOOL_EXPORT char *joinPath(const char *path, const char *name, const char *suffix);
AFUN_TOOL_EXPORT char *getFileName(const char *path_1);
AFUN_TOOL_EXPORT char *getFileNameWithPath(const char *path_1);
AFUN_TOOL_EXPORT char *getFilePath(const char *path_1, int dep);
AFUN_TOOL_EXPORT char *getFileSurfix(const char *path);
AFUN_TOOL_EXPORT char *fileNameToVar(char *name, bool need_free);
AFUN_TOOL_EXPORT char *findPath(char *path, const char *env, bool need_free);
AFUN_TOOL_EXPORT char *getExedir(int dep);
AFUN_TOOL_EXPORT uintmax_t getFileSize(const char *path);
AFUN_TOOL_EXPORT bool isCharUTF8(const char *str);
AFUN_TOOL_EXPORT FILE *fileOpen(const char *path_, const char *mode_);
AFUN_TOOL_EXPORT int fileClose(FILE *file);
#endif //AFUN_FILE_HPP
