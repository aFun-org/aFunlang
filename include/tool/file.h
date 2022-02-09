#ifndef AFUN_FILE_H
#define AFUN_FILE_H
#include "iostream"
#include "aFunToolExport.h"

/* 文件处理工具 */
namespace aFuntool {
    AFUN_TOOL_EXPORT int checkFile(const std::string &path);
    AFUN_TOOL_EXPORT time_t getFileMTime(const std::string &path);
    AFUN_TOOL_EXPORT uintmax_t getFileSize(const std::string &path);
    AFUN_TOOL_EXPORT FILE *fileOpen(const std::string &path_, const char *mode_);
    AFUN_TOOL_EXPORT FILE *fileOpen(const char *path_, const char *mode_);
    AFUN_TOOL_EXPORT int fileClose(FILE *file);
}

#endif //AFUN_FILE_H
