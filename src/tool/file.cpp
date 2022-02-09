#include "tool-type.h"
#include "file.h"
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
    int get_stat(aFun_stat &stat_, const std::string &path_){
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