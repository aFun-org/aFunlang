#ifndef AFUN_LOG_H
#define AFUN_LOG_H
#include <setjmp.h>
#include "aFunToolExport.h"
#include "macro.h"

enum LogLevel {
    log_debug = 0,
    log_info = 1,
    log_warning = 2,
    log_error = 3,
    log_send_error = 4,
    log_fatal_error = 5,
};
typedef enum LogLevel LogLevel;

enum LogFactoryPrintConsole {
    log_pc_all = 1,
    log_pc_w = 2,
    log_pc_e = 3,
    log_pc_quite = 4,
};
typedef enum LogFactoryPrintConsole LogFactoryPrintConsole;

enum LogLoggerPrintConsole {
    log_default = 0,  // 默认
    log_print_console,  // 不显示到终端
    log_print_no_console,  // 只显示到终端
};
typedef enum LogLoggerPrintConsole LogLoggerPrintConsole;

struct Logger {
    char *id;
    LogLevel level;
    bool process_send_error;  // 若为false则send_error转换为error
    bool process_fatal_error;  // 若为false则fatal_error转换为error
    jmp_buf *buf;
    int exit_type;    // 0-abort 其他值则为exit
};
typedef struct Logger Logger;

AFUN_TOOL_EXPORT int initLogSystem(FilePath path, LogFactoryPrintConsole print_console);

AFUN_TOOL_EXPORT void initLogger(Logger *logger, char *id, LogLevel level);
AFUN_TOOL_EXPORT void destructLogger(Logger *logger);

AFUN_TOOL_EXPORT int writeDebugLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeInfoLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeWarningLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeErrorLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeSendErrorLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeFatalErrorLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, int exit_code, char *format, ...);

#ifdef __FILENAME__
#define file_line (char *)__FILENAME__ , (int)__LINE__
#else
#define file_line (char *)"xx", (int)__LINE__
#endif

#define writeDebugLog(logger, pc, ...) writeDebugLog_(logger, pc, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeInfoLog(logger, pc, ...) writeInfoLog_(logger, pc, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeWarningLog(logger, pc, ...) writeWarningLog_(logger, pc, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeErrorLog(logger, pc, ...) writeErrorLog_(logger, pc, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeSendErrorLog(logger, pc, ...) writeSendErrorLog_(logger, pc, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeFatalErrorLog(logger, pc, exit_code, ...) writeFatalErrorLog_(logger, pc, file_line, (char *)__FUNCTION__, exit_code, __VA_ARGS__)

#define assertDebugLog(c, logger, pc, ...) ((c) || writeDebugLog(logger, pc, "Assert " #c " error : " __VA_ARGS__))
#define assertInfoLog(c, logger, pc, ...) ((c) || writeInfoLog(logger, pc, "Assert " #c " error : " __VA_ARGS__))
#define assertWarningLog(c, logger, pc, ...) ((c) || writeWarningLog(logger, pc, "Assert " #c " error : " __VA_ARGS__))
#define assertErrorLog(c, logger, pc, ...) ((c) || writeErrorLog(logger, pc, "Assert " #c " error : " __VA_ARGS__))

#endif //AFUN_LOG_H
