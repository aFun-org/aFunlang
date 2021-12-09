﻿/*
 * 文件名: file.c
 * 目标: 关于文件读取的实用函数
 */

#include <sys/stat.h>
#include <cctype>
#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "tool.hpp"

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

#ifdef aFunWIN32_NO_CYGWIN
typedef struct _stat64 aFun_stat;
typedef wchar_t aFun_path;
#else
typedef struct stat aFun_stat;
typedef char aFun_path;
#endif

static int get_stat(aFun_stat *stat_, const char *path_){
    int re;
#ifdef aFunWIN32_NO_CYGWIN
    aFun_path *tmp = nullptr;
    if (convertWideByte(&tmp, path_, CP_UTF8) == 0)
        return -1;
    re = _wstat64(tmp, stat_);
    free(tmp);  // 如果 path 为nullptr, 则释放最新生成的 wchat_t
#else
    re = stat(path_, stat_);
#endif
    return re;
}

/*
 * 函数名: checkFile
 * 目标判断文件类型, 若是普通文件返回1, 若是文件夹返回2, 其他遇到错误返回0
 */
int checkFile(const char *path_){
    if (path_ == nullptr)
        return 0;

    int re = 0;
    aFun_stat stat;
    if (get_stat(&stat, path_) != 0)
        re = 0;
    else if (S_ISREG(stat.st_mode))  // 普通文件
        re = 1;
    else if (S_ISDIR(stat.st_mode))
        re = 2;
    return re;
}

time_t getFileMTime(const char *path) {
    aFun_stat stat;
    if (path == nullptr || get_stat(&stat, path) != 0)
        return 0;
    return stat.st_mtime;
}

char *joinPath(const char *path, const char *name, const char *suffix) {
    char *name_suffix = strJoin(name, suffix, false, false);
    char *res;

    if (path != nullptr && path[STR_LEN(path) - 1] == SEP_CH)
        res = strJoin(path, name_suffix, false, true);
    else if (path != nullptr) {
        res = strJoin(path, SEP, false, false);
        res = strJoin(res, name_suffix, true, true);
    } else
        res = name_suffix;

    /* name_suffix已经在上述的strJoin释放 */
    return res;
}

/*
 * 函数: getFileName
 * 目标: 给定路径获取该路径所指定的文件名
 */
char *getFileName(const char *path_1){
    char *path = strCopy(path_1);  // 复制数组, 避免path_1是常量字符串导致无法修改其值
    char *slash;  // 名字开始的字符的指针
    char *point;  // 后缀名.所在的字符的指针

    if (path[STR_LEN(path) - 1] == SEP_CH)  // 若路径的最后一个字符为SEP, 则忽略此SEP
        path[STR_LEN(path) - 1] = NUL;

    if ((slash = strrchr(path, SEP_CH)) == nullptr)
        slash = path;
    else
        slash++;

    if ((point = getFileSurfix(path)) != nullptr)
        *point = NUL;

    char *res = strCopy(slash);
    free(path);
    return res;
}

/*
 * 函数名: getFileNameWithPath
 * 目标: 取出指定路径的文件后缀
 */
char *getFileNameWithPath(const char *path_1){
    char *path = strCopy(path_1);  // 复制数组, 避免path_1是常量字符串导致无法修改其值
    char *point;  // 后缀名.所在的字符的指针
    char *res;

    if ((point = getFileSurfix(path)) != nullptr)
        *point = NUL;

    res = strCopy(path);
    free(path);
    return res;
}

char *getFilePath(const char *path_1, int dep){
    char *path = strCopy(path_1);  // 复制数组, 避免path_1是常量字符串导致无法修改其值
    char *slash;  // 后缀名.所在的字符的指针
    char *res;

    if (path[STR_LEN(path) - 1] == SEP_CH)  // 若路径的最后一个字符为SEP, 则忽略此SEP
        path[STR_LEN(path) - 1] = NUL;

    for(NULL; dep > 0; dep--) {
        if ((slash = strrchr(path, SEP_CH)) != nullptr)
            *slash = NUL;
    }

    res = strCopy(path);
    free(path);
    return res;
}

/*
 * 函数名: getFileSurfix
 * 目标: 获取文件后缀 (不会生成新字符串)
 */
char *getFileSurfix(const char *path) {
    const char *last_ = strrchr(path, SEP_CH);
    char *ret;

    if (last_ != nullptr)
        ret = const_cast<char *>(strrchr(last_ + 1, '.'));
    else
        ret = const_cast<char *>(strrchr(path, '.'));
    return ret;
}

/*
 * 函数名: fileNameToVar
 * 目标: 把一个文件名转换为合法的变量名(替换不合法字符为_)
 */
char *fileNameToVar(char *name, bool need_free){
    char *var;
    if (need_free)
        var = name;  // 在原数据上修改
    else
        var = strCopy(name);  // 复制新的数据再修改

    if (!isalpha(*var) && *var != '_')
        var = strJoin("_", var, false, true);
    for (char *tmp = var; *tmp != 0; tmp++)
        if (!isalnum(*tmp) &&'_' != *tmp)
            *tmp = '_';
    return var;
}

/*
 * 函数名: findPath
 * 目标: 转换路径为合法路径（相对路径->绝对路径, 绝对路径保持不变）
 */
char *findPath(char *path, const char *env, bool need_free){
    assert(env[STR_LEN(env) - 1] == SEP_CH);  // env 必须以 SEP 结尾
#ifdef __linux
    if (path[0] != SEP_CH) { // 不以 / 开头
#else
    if (!(isupper(path[0]) && (path)[1] == ':')) {  // 不以盘符开头
#endif
        return strJoin(env, path, false, need_free);  // 调整为相对路径模式
    } else if (!need_free) { // 若设置了need_free为true, 则等于先复制在释放原来的, 等于没有复制， 所以不做操作
        return strCopy(path);
    } else {
        return path;
    }
}

/*
 * 函数名: 获取可执行程序目录
 * dep表示从可执行程序往回跳出的层数
 */
char *getExedir(int dep) {
    aFun_path exepath[218] = {0};
#ifdef aFunWIN32_NO_CYGWIN
    DWORD ret = GetModuleFileNameW(nullptr, exepath, 217);  // 预留一位给NUL
    if (ret == 0 || WSTR_LEN(exepath) == 0)
        return nullptr;
    char *path = nullptr;
    if (convertFromWideByte(&path, exepath, CP_UTF8) == 0)
        return nullptr;
    char *re = getFilePath(path, dep + 1);
    free(path);
    return re;
#else
    ssize_t ret =  readlink("/proc/self/exe", exepath, 217);  // 预留一位给NUL
    if (ret == -1 || STR_LEN(exepath) == 0)
        return nullptr;
    return getFilePath(exepath, dep + 1);
#endif
}

uintmax_t getFileSize(const char *path) {
    aFun_stat stat;
    int ret;
    ret = get_stat(&stat, path);
    if(ret != 0)
        return 0;  // 获取失败。
    return (uintmax_t)stat.st_size;  // 返回文件大小

}

/*
 * 函数名: isCharUTF8
 * 目标: 检查给定字符串是否utf-8编码
 */
bool isCharUTF8(const char *str) {
    int code = 0;  // utf-8 多字节数
    for (const char *ch = str; *ch != NUL; ch++) {
        unsigned char c = *ch;
        unsigned char c_ = ~c;

        assertFatalErrorLog(code >= 0 && code <= 5, nullptr, 2, "str = %s", str);
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

FILE *fileOpen(const char *path_, const char *mode_) {
    if (STR_LEN(mode_) >= 5)
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
    free(path);
    return file;
#else
    return fopen(path_, mode_);
#endif
}

int fileClose(FILE *file) {
    return fclose(file);
}