#ifndef AFUN_TOOL_LOGGER_H
#define AFUN_TOOL_LOGGER_H
#include "aFunToolExport.h"

namespace aFuntool {
    class Logger;
    extern AFUN_TOOL_EXPORT class Logger *aFunSysLogger;

    static void setSysLogger(aFuntool::Logger *log);
}

#include "tool-logger.inline.h"

#endif //AFUN_TOOL_LOGGER_H
