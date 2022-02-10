#ifndef AFUN_TIME_H
#define AFUN_TIME_H
#include "aFunToolExport.h"

#if __cplusplus
#include <ctime>
#else
#include "time.h"
#endif

/* 时间工具 */
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_TOOL_C_EXPORT_FUNC void safeSleep(double ms);
    AFUN_TOOL_C_EXPORT_FUNC char *getTime(time_t *t, const char *format);

#ifdef __cplusplus
    AFUN_TOOL_EXPORT std::string getTime(time_t *t, const std::string &format);
#endif

#ifndef AFUN_TOOL_C
}
#endif
#endif //AFUN_TIME_H
