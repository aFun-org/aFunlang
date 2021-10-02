#ifndef AFUN_INIT_H
#define AFUN_INIT_H
#include "aFunCoreExport.h"
#include "stdbool.h"
#include <setjmp.h>
#include "tool.h"

AFUN_CORE_EXPORT extern Logger *aFunCoreLogger;
AFUN_CORE_EXPORT bool aFunCoreInit(char *log_dir, LogFactoryPrintConsole print_console, bool fe, bool se, jmp_buf *buf, LogLevel level);

#endif //AFUN_INIT_H
