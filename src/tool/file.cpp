/*
 * 文件名: file.c
 * 目标: 关于文件读取的实用函数
 */

#include <sys/stat.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include "tool-type.h"
#include "file.h"
#include "path.h"
#include "str.h"
#include "log.h"
#include "tool-stdio.h"

#ifdef aFunWIN32_NO_CYGWIN
#ifdef _MSC_VER
#pragma warning(disable : 5105)  // 关闭 5105 的警告输出 (Windows.h中使用)
#endif
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifndef S_ISREG
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif

#ifndef S_ISDIR
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif

namespace aFuntool {
#ifdef aFunWIN32_NO_CYGWIN
    typedef struct _stat64 aFun_stat;
    typedef wchar_t aFun_path;
#else
    typedef struct stat aFun_stat;
    typedef char aFun_path;
#endif

    /**
     * 获取文件的stat结构体
     * @param stat stat 保存地址
     * @param path 路径 (utf-8)
     * @return
     */
    static int get_stat(aFun_stat &stat_, const std::string &path_){
        int re;
#ifdef aFunWIN32_NO_CYGWIN
        aFun_path *tmp = nullptr;
        if (convertWideByte(&tmp, path_.c_str(), CP_UTF8) == 0)
            return -1;
        re = _wstat64(tmp, &stat_);
        safeFree(tmp);  // 如果 path 为nullptr, 则释放最新生成的 wchat_t
#else
        re = stat(path_.c_str(), &stat_);
#endif
        return re;
    }

    /**
     * 目标判断文件类型, 若是普通文件返回1, 若是文件夹返回2, 其他遇到错误返回0
     */
    int checkFile(const std::string &path){
        if (path.empty())
            return 0;

        int re = 0;
        aFun_stat stat;
        if (get_stat(stat, path) != 0)
            re = 0;
        else if (S_ISREG(stat.st_mode))  // 普通文件
            re = 1;
        else if (S_ISDIR(stat.st_mode))
            re = 2;
        return re;
    }

    /**
     * 获取文件最后修改时间
     */
    time_t getFileMTime(const std::string &path) {
        aFun_stat stat;
        if (path.empty() || get_stat(stat, path) != 0)
            return 0;
        return stat.st_mtime;
    }

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
    std::string getFileSurfix(const std::string &path) {
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
    std::string getHomePath() {
        aFun_path exe_path[218] = {0};
#ifdef aFunWIN32_NO_CYGWIN
        DWORD ret = GetModuleFileNameW(nullptr, exe_path, 217);  // 预留一位给NUL
        if (ret == 0 || wcslen(exe_path) == 0)
            return "";
        char *path = nullptr;
        if (convertFromWideByte(&path, exe_path, CP_UTF8) == 0)
            return "";
        std::string re = getFilePath(path, 2);
        safeFree(path);
        return re;
#else
        ssize_t ret =  readlink("/proc/self/exe", exe_path, 217);  // 预留一位给NUL
        if (ret == -1 || strlen(exe_path) == 0)
            return "";
        return getFilePath(exe_path, 2);
#endif
    }

    /**
     * 获取可执行程序目录
     * @param dep 从可执行程序往回跳出的层数
     */
    std::string getExePath() {
        aFun_path exe_path[218] = {0};
#ifdef aFunWIN32_NO_CYGWIN
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

    /**
     * @param path 文件路径 (utf-8)
     * @return 文件大小
     */
    uintmax_t getFileSize(const std::string &path) {
        aFun_stat stat;
        int ret;
        ret = get_stat(stat, path);
        if(ret != 0)
            return 0;  // 获取失败。
        return (uintmax_t)stat.st_size;  // 返回文件大小
    }

    /**
     * 检查给定字符串是否utf-8编码
     * @param str 字符串
     */
    bool isCharUTF8(const char *str) {
        int code = 0;  // utf-8 多字节数
        for (const char *ch = str; *ch != NUL; ch++) {
            unsigned char c = *ch;
            unsigned char c_ = ~c;

            assertFatalErrorLog(code >= 0 && code <= 5, aFunSysLogger, 2, "str = %s", str);
            if (code == 0) {
                if ((c_ & 0xFC) == 0 && (c & 0x02) == 0)  // 检查是否为1111110x, 先对其取反, 使用0xFC掩码检查前6位是否为0, 然后单独检查倒数第二位是否为0
                    code = 5;  // 剩余 5 个字节
                else if ((c_ & 0xF8) == 0 && (c & 0x04) == 0)
                    code = 4;  // 剩余 4 个字节
                else if ((c_ & 0xF0) == 0 && (c & 0x08) == 0)
                    code = 3;  // 剩余 3 个字节
                else if ((c_ & 0xE0) == 0 && (c & 0x10) == 0)
                    code = 2;  // 剩余 2 个字节
                else if ((c_ & 0xC0) == 0 && (c & 0x20) == 0)
                    code = 1;  // 剩余 1 个字节
                else if ((c & 0x80) == 0)  // 检查最高位是否为0
                    code = 0;
                else
                    return false;
            } else if ((c_ & 0x80) == 0 && (c & 0x40) == 0)
                code--;
            else
                return false;
        }

        return true;
    }

    bool isCharUTF8(const std::string &str) {
        return isCharUTF8(str.c_str());
    }

    /**
     * 打开指定文件
     * @param path_ 路径 (utf-8)
     * @param mode_ 模式
     * @return
     */
    FILE *fileOpen(const char *path_, const char *mode_) {
        if (strlen(mode_) >= 5)
            return nullptr;
#ifdef aFunWIN32_NO_CYGWIN
        FILE *file = nullptr;
        wchar_t *path = nullptr;
        wchar_t mode[5];
        if (convertWideByte(&path, path_, CP_UTF8) == 0)
            return nullptr;
        for (int i = 0; i < 5; i++)
            mode[i] = (wchar_t)mode_[i];  // ascii字符转换

        _wfopen_s(&file, path, mode);
        safeFree(path);
        return file;
#else
        return fopen(path_, mode_);
#endif
    }

    FILE *fileOpen(const std::string &path_, const char *mode_) {
        return fileOpen(path_.c_str(), mode_);
    }

    /**
     * 关闭文件, 本质和fclose一样
     * @param file FILE
     * @return
     */
    int fileClose(FILE *file) {
        return fclose(file);
    }
}