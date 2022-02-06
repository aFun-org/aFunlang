#ifndef AFUN_TOOL_LOGGER_INLINE_H
#define AFUN_TOOL_LOGGER_INLINE_H
#include "tool-logger.h"

namespace aFuntool {
    static void setSysLogger(aFuntool::Logger *log) {
        aFunSysLogger = log;
    }
}

#endif //AFUN_TOOL_LOGGER_INLINE_H
