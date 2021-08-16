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