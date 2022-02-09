#ifndef AFUN_DIRECTORY_H
#define AFUN_DIRECTORY_H
#include "aFunToolExport.h"
#include "iostream"

/* 文件处理工具 */
namespace aFuntool {
    AFUN_TOOL_EXPORT std::string joinPath(const std::string &path, const std::string &name, const std::string &suffix);
    AFUN_TOOL_EXPORT std::string getFileName(const std::string &path);
    AFUN_TOOL_EXPORT std::string getFilePathName(const std::string &path);
    AFUN_TOOL_EXPORT std::string getFilePath(const std::string &path_1, int dep);
    AFUN_TOOL_EXPORT std::string getFileSuffix(const std::string &path);
    AFUN_TOOL_EXPORT std::string fileNameToVar(const std::string &name);
    AFUN_TOOL_EXPORT std::string findPath(const std::string &path, const std::string &env);
    AFUN_TOOL_EXPORT std::string getExePath();
}
#endif //AFUN_DIRECTORY_H
