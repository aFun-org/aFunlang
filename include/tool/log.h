#ifndef AFUN_LOG_H
#define AFUN_LOG_H
#include "aFunToolExport.h"
#include <setjmp.h>

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

struct Logger {
    char *id;
    LogLevel level;
    bool process_send_error;  // 若为false则send_error转换为error
    bool process_fatal_error;  // 若为false则fatal_error转换为error
    jmp_buf *buf;
};
typedef struct Logger Logger;

AFUN_TOOL_EXPORT int initLogSystem(FilePath path, LogFactoryPrintConsole print_console);

AFUN_TOOL_EXPORT void initLogger(Logger *logger, char *id, LogLevel level);
AFUN_TOOL_EXPORT void destructLogger(Logger *logger);

AFUN_TOOL_EXPORT int writeDebugLog_(Logger *logger, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeInfoLog_(Logger *logger, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeWarningLog_(Logger *logger, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeErrorLog_(Logger *logger, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeSendErrorLog_(Logger *logger, char *file, int line, char *func, char *format, ...);
AFUN_TOOL_EXPORT int writeFatalErrorLog_(Logger *logger, char *file, int line, char *func, int exit_code, char *format, ...);

#ifdef __FILENAME__
#define file_line (char *)__FILENAME__ , (int)__LINE__
#else
#define file_line (char *)"xx", (int)__LINE__
#endif

#define writeDebugLog(logger, ...) writeDebugLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeInfoLog(logger, ...) writeInfoLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeWarningLog(logger, ...) writeWarningLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeErrorLog(logger, ...) writeErrorLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeSendErrorLog(logger, ...) writeSendErrorLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeFatalErrorLog(logger, exit_code, ...) writeFatalErrorLog_(logger, file_line, (char *)__FUNCTION__, exit_code, __VA_ARGS__)

#endif //AFUN_LOG_H
