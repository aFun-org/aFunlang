/*
 * 文件名: time.c
 * 目标: 关于time的实用函数
 */

#include <ctime>
#include "tool.hpp"

/*
 * 函数名: safe_sleep
 * 目标: 等待指定的秒数(ms) 支持小数
 */
void safeSleep(double ms) {
    time_t start = clock();
    time_t now;
    time_t d_time;
    auto ms_t = (time_t) (ms * CLOCKS_PER_SEC);
    do {
        now = clock();
        d_time = now - start;
    } while (d_time < ms_t);
}

/*
 * 函数名: getTime
 * 目标: 格式化输出时间
 * 注意: 该函数不可以使用log模块
 */
char *getTime(time_t *t, const char *format) {
    time_t tmp;
    if (t == nullptr)
        t = &tmp;

    time (t);  // 获取时间戳
#ifdef aFunWIN32_NO_CYGWIN
    struct tm lt{};
    if (localtime_s(&lt, t) != 0)
        return nullptr;
    wchar_t time_str[100];
    wchar_t *format_ = nullptr;
    if (convertWideByte(&format_, format, CP_UTF8) == 0)
        return nullptr;
    wcsftime(time_str, 100, format_, &lt);
    free(format_);

    char *re = nullptr;
    if (convertFromWideByte(&re, time_str, CP_UTF8) == 0)
        return nullptr;
    return re;
#else
    struct tm *lt = nullptr;
    lt = localtime (t);  // 转为时间结构
    char time_str[100];
    strftime(time_str, 100, format, lt);
    return strCopy(time_str);
#endif
}
