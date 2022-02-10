#ifndef AFUN_FILE_H
#define AFUN_FILE_H
#include "aFunToolExport.h"

#ifdef __cplusplus
#include <iostream>
#else
#include <stdio.h>
#endif

/* 文件处理工具 */
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

#ifdef __cplusplus
    AFUN_TOOL_C_EXPORT_FUNC int checkFile(const std::string &path);
    AFUN_TOOL_C_EXPORT_FUNC time_t getFileMTime(const std::string &path);
    AFUN_TOOL_C_EXPORT_FUNC uintmax_t getFileSize(const std::string &path);
    AFUN_TOOL_EXPORT FILE *fileOpen(const std::string &path_, const char *mode_);
#endif
    AFUN_TOOL_C_EXPORT_FUNC FILE *fileOpen(const char *path_, const char *mode_);
    AFUN_TOOL_C_EXPORT_FUNC int fileClose(FILE *file);

#ifndef AFUN_TOOL_C
}
#endif

#endif //AFUN_FILE_H
