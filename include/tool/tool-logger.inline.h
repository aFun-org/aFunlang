#ifndef AFUN_TOOL_LOGGER_INLINE_H
#define AFUN_TOOL_LOGGER_INLINE_H
#ifdef __cplusplus

#include "tool-logger.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    void setSysLogger(Logger *log) {
        aFunSysLogger = log;
    }

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_TOOL_LOGGER_INLINE_H
