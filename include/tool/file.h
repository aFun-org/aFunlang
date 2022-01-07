#ifndef AFUN_FILE_H
#define AFUN_FILE_H
#include "aFunToolExport.h"

/* 文件处理工具 */
namespace aFuntool {
    AFUN_TOOL_EXPORT int checkFile(const std::string &path);
    AFUN_TOOL_EXPORT time_t getFileMTime(const std::string &path);
    AFUN_TOOL_EXPORT std::string joinPath(const std::string &path, const std::string &name, const std::string &suffix);
    AFUN_TOOL_EXPORT std::string getFileName(const std::string &path);
    AFUN_TOOL_EXPORT std::string getFilePathName(const std::string &path);
    AFUN_TOOL_EXPORT std::string getFilePath(const std::string &path_1, int dep);
    AFUN_TOOL_EXPORT std::string getFileSurfix(const std::string &path);
    AFUN_TOOL_EXPORT std::string fileNameToVar(const std::string &name);
    AFUN_TOOL_EXPORT std::string findPath(const std::string &path, const std::string &env);
    AFUN_TOOL_EXPORT std::string getExedir(int dep);
    AFUN_TOOL_EXPORT uintmax_t getFileSize(const std::string &path);
    AFUN_TOOL_EXPORT bool isCharUTF8(const char *str);
    AFUN_TOOL_EXPORT bool isCharUTF8(const std::string &str);
    AFUN_TOOL_EXPORT FILE *fileOpen(const std::string &path_, const char *mode_);
    AFUN_TOOL_EXPORT FILE *fileOpen(const char *path_, const char *mode_);
    AFUN_TOOL_EXPORT int fileClose(FILE *file);
}
#endif //AFUN_FILE_H
