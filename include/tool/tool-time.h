#ifndef AFUN_TIME_H
#define AFUN_TIME_H
#include "aFunToolExport.h"

/* 时间工具 */
namespace aFuntool {
    AFUN_TOOL_EXPORT void safeSleep(double ms);
    AFUN_TOOL_EXPORT char *getTime(time_t *t, const char *format);
    AFUN_TOOL_EXPORT std::string getTime(time_t *t, const std::string &format);
}
#endif //AFUN_TIME_H
