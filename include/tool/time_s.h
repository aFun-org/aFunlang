#ifndef AFUN_TIME_H
#define AFUN_TIME_H
#include "aFunToolExport.h"

#ifdef aFunWin32

#else
#include <unistd.h>
#endif

/* 时间工具 */
AFUN_TOOL_EXPORT void safeSleep(double ms);
AFUN_TOOL_EXPORT char *getTime(time_t *t, char *format);
#endif //AFUN_TIME_H
