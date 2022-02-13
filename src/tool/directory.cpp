/*
 * 文件名: file.c
 * 目标: 关于文件读取的实用函数
 */

#include <cctype>

#include "tool_directory.h"
#include "tool_path.h"
#include "tool_str.h"
#include "tool_log.h"
#include "tool_stdio.h"

#ifdef AFUN_WIN32_NO_CYGWIN
#ifdef _MSC_VER
#pragma warning(disable : 5105)  // 关闭 5105 的警告输出 (Windows.h中使用)
#endif
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace aFuntool {
#ifdef AFUN_WIN32_NO_CYGWIN
    typedef wchar_t aFun_path;
#else
    typedef char aFun_path;
#endif

    /**
     * 拼接路径
     * @param path 路径
     * @param name 文件名
     * @param suffix 后缀
     * @return
     */
    std::string joinPath(const std::string &path, const std::string &name, const std::string &suffix) {
        std::string name_suffix = name + suffix;
        if (!path.empty() && *(path.end()) == SEP_CH)
            return path + name_suffix;
        else if (!path.empty())
            return path + SEP + name_suffix;
        return name_suffix;
    }

    /**
     * 给定路径获取该路径所指定的文件名
     */
    std::string getFileName(const std::string &path){
        int sep = 0;
        if (*(path.end()) == SEP_CH)  // 若路径的最后一个字符为SEP, 则忽略此SEP
            sep = -1;

        auto slash = path.find_last_of('/');
        if (slash == std::string::npos)
            slash = 0;
        else
            slash++;

        return path.substr(path.size() - slash + sep, slash);
    }

    /**
     * 获取 文件路径+文件名（排除后缀）
     */
    std::string getFilePathName(const std::string &path){
        auto point = path.find_last_of('.');
        if (point == std::string::npos)
            return path;
        return path.substr(point);
    }

    /**
     * 获取文件路径(不包含文件名)
     */
    std::string getFilePath(const std::string &path, int dep){
        std::string::size_type point = path.size();
        for (int i = 0; i < dep; i++) {
            auto tmp = path.rfind(SEP_CH, point - 1);
            if (tmp == std::string::npos)
                break;
            point = tmp;
        }
        return path.substr(0, point);
    }

    /**
     * 获取文件后缀
     */
    std::string getFileSuffix(const std::string &path) {
        auto point = path.find_last_of('.');
        if (point == std::string::npos)
            point = 0;
        else
            point++;

        std::string ret = path.substr(path.size() - point, point);
        return ret;
    }

    /**
     * 把一个文件名转换为合法的变量名(替换不合法字符为_)
     * @param name 路径
     * @param need_free 是否需要释放
     */
    std::string fileNameToVar(const std::string &name){
        char *var = strCopy(name.c_str());  // 复制新的数据再修改

        if (!isalpha(*var) && *var != '_')
            var = strJoin("_", var, false, true);
        for (char *tmp = var; *tmp != 0; tmp++)
            if (!isalnum(*tmp) &&'_' != *tmp)
                *tmp = '_';
        std::string ret = var;
        safeFree(var);
        return ret;
    }

    /**
     * 转换路径为合法路径（相对路径->绝对路径, 绝对路径保持不变）
     * @param path 文件路径
     * @param env 环境 必须以 / 结尾
     * @param need_free 是否需要释放 path
     */
    std::string findPath(const std::string &path, const std::string &env){
#ifdef __linux
        if (path[0] != SEP_CH) // 不以 / 开头
#else
        if (!(isupper(path[0]) && (path)[1] == ':'))  // 不以盘符开头
#endif
            return env + path;  // 调整为相对路径模式
        return path;
    }

    /**
     * 获取可执行程序目录
     * @param dep 从可执行程序往回跳出的层数
     */
    std::string getExePath() {
        aFun_path exe_path[218] = {0};
#ifdef AFUN_WIN32_NO_CYGWIN
        DWORD ret = GetModuleFileNameW(nullptr, exe_path, 217);  // 预留一位给NUL
        if (ret == 0 || wcslen(exe_path) == 0)
            return "";
        char *path = nullptr;
        if (convertFromWideByte(&path, exe_path, CP_UTF8) == 0)
            return "";
        std::string re = path;
        safeFree(path);
        return re;
#else
        ssize_t ret =  readlink("/proc/self/exe", exe_path, 217);  // 预留一位给NUL
        if (ret == -1 || strlen(exe_path) == 0)
            return "";
        return exe_path;
#endif
    }
}