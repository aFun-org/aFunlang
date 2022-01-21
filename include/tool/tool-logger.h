#ifndef AFUN_TOOL_LOGGER_H
#define AFUN_TOOL_LOGGER_H
#include "aFunToolExport.h"

namespace aFuntool {
    class Logger;
    extern AFUN_TOOL_EXPORT class Logger *SysLogger;

    static void setSysLogger(aFuntool::Logger *log) {
        SysLogger = log;
    }
}

#endif //AFUN_TOOL_LOGGER_H
