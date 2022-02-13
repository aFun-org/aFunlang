/*
 * 文件名: time.c
 * 目标: 关于time的实用函数
 */

#include <ctime>
#include "tool.h"
#include "tool_time.h"
#include "tool_stdio.h"
#include "tool_str.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    /**
     * 等待指定的秒数(ms) 支持小数
     */
    void safeSleep(double ms){
        time_t start = clock();
        time_t now;
        time_t d_time;
        auto ms_t = (time_t) (ms * CLOCKS_PER_SEC);
        do {
            now = clock();
            d_time = now - start;
        } while (d_time < ms_t);
    }

    /**
     * 格式化输出时间
     * 注意: 该函数不可以使用log模块
     */
    char *getTime(time_t *t, const char *format){
        time_t tmp;
        if (t == nullptr)
            t = &tmp;

        time(t);  // 获取时间戳
#ifdef AFUN_WIN32_NO_CYGWIN
        struct tm lt{};
        if (localtime_s(&lt, t) != 0)
            return nullptr;
        wchar_t time_str[100];
        wchar_t *format_ = nullptr;
        if (convertWideByte(&format_, format, CP_UTF8) == 0)
            return nullptr;
        wcsftime(time_str, 100, format_, &lt);
        safeFree(format_);

        char *re = nullptr;
        if (convertFromWideByte(&re, time_str, CP_UTF8) == 0)
            return nullptr;
        return re;
#else
        struct tm *lt = localtime (t);
        char time_str[100];
        strftime(time_str, 100, format, lt);
        return strCopy(time_str);
#endif
    }

    std::string getTime(time_t *t, const std::string &format){
        time_t tmp;
        if (t == nullptr)
            t = &tmp;

        time(t);  // 获取时间戳
#ifdef AFUN_WIN32_NO_CYGWIN
        struct tm lt{};
        if (localtime_s(&lt, t) != 0)
            return "";
        wchar_t time_str[100];
        wchar_t *format_ = nullptr;
        if (convertWideByte(&format_, format.c_str(), CP_UTF8) == 0)
            return "";
        wcsftime(time_str, 100, format_, &lt);
        safeFree(format_);

        char *tmp_ch = nullptr;
        if (convertFromWideByte(&tmp_ch, time_str, CP_UTF8) == 0)
            return "";
#else
        struct tm *lt = localtime (t);
        char time_str[100];
        strftime(time_str, 100, format.c_str(), lt);
        char *tmp_ch = strCopy(time_str);;
#endif
        std::string ret = tmp_ch;
        safeFree(tmp_ch);
        return ret;
    }

#ifndef AFUN_TOOL_C
}
#endif
