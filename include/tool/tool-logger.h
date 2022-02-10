#ifndef AFUN_TOOL_LOGGER_H
#define AFUN_TOOL_LOGGER_H
#ifdef __cplusplus

#include "aFunToolExport.h"
#include "tool-macro.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    class Logger;
    extern AFUN_TOOL_EXPORT class Logger *aFunSysLogger;

    AFUN_STATIC void setSysLogger(Logger *log);

#ifndef AFUN_TOOL_C
}
#endif

#include "tool-logger.inline.h"

#endif
#endif //AFUN_TOOL_LOGGER_H
