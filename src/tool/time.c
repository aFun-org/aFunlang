/*
 * 文件名: time.c
 * 目标: 关于time的实用函数
 */

#include <time.h>
#include "tool.h"

/*
 * 函数名: safe_sleep
 * 目标: 等待指定的秒数(ms) 支持小数
 */
void safeSleep(double ms) {
    time_t start = clock();
    time_t now;
    time_t d_time;
    time_t ms_t = (time_t) (ms * CLOCKS_PER_SEC);
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
char *getTime(time_t *t, char *format) {
    time_t tmp;
    if (t == NULL)
        t = &tmp;

    time (t);  // 获取时间戳
#ifdef aFunWIN32_NO_CYGWIN
    struct tm lt;
    if (localtime_s(&lt, t) != 0)
        return NULL;
    wchar_t time_str[100];
    wchar_t *format_ = NULL;
    if (convertWideByte(&format_, format, CP_UTF8) == 0)
        return NULL;
    wcsftime(time_str, 100, format_, &lt);
    free(format_);

    char *re = NULL;
    if (convertFromWideByte(&re, time_str, CP_UTF8) == 0)
        return NULL;
    return re;
#else
    struct tm *lt = NULL;
    lt = localtime (t);  // 转为时间结构
    char time_str[100];
    strftime(time_str, 100, format, lt);
    return strCopy(time_str);
#endif
}
