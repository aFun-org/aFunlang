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
char *getTime(time_t *t) {
    time_t tmp;
    if (t == NULL)
        t = &tmp;

    struct tm *lt;
    time (t);  // 获取时间戳
    lt = localtime (t);  // 转为时间结构
    char time_str[100];
    snprintf(time_str, 100, "%d/%d/%d %d:%d:%d",
             lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    return strCopy(time_str);
}
