#ifndef AFUN_LOG_H
#define AFUN_LOG_H
#include <setjmp.h>
#include "aFunToolExport.h"
#include "macro.h"

enum LogLevel {
    log_track = 0,
    log_debug = 1,
    log_info = 2,
    log_warning = 3,
    log_error = 4,
    log_send_error = 5,
    log_fatal_error = 6,
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
    jmp_buf *buf;
    int exit_type;    // 0-abort 其他值则为exit
};
typedef struct Logger Logger;

AFUN_TOOL_EXPORT int initLogSystem(FilePath path);

AFUN_TOOL_EXPORT void initLogger(Logger *logger, char *id, LogLevel level);
AFUN_TOOL_EXPORT void destructLogger(Logger *logger);

AFUN_TOOL_EXPORT int writeTrackLog_(Logger *logger, char *file, int line, char *func, char *format, ...);
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

#if aFunDEBUG
#define writeTrackLog(logger, ...) writeTrackLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#else
#define writeTrackLog(logger, ...) (NULL)
#endif

#define writeDebugLog(logger, ...) writeDebugLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeInfoLog(logger, ...) writeInfoLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeWarningLog(logger, ...) writeWarningLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeErrorLog(logger, ...) writeErrorLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeSendErrorLog(logger, ...) writeSendErrorLog_(logger, file_line, (char *)__FUNCTION__, __VA_ARGS__)
#define writeFatalErrorLog(logger, exit_code, ...) writeFatalErrorLog_(logger, file_line, (char *)__FUNCTION__, exit_code, __VA_ARGS__)

#define assertDebugLog(c, logger, ...) ((c) || writeDebugLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertInfoLog(c, logger, ...) ((c) || writeInfoLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertWarningLog(c, logger, ...) ((c) || writeWarningLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertErrorLog(c, logger, ...) ((c) || writeErrorLog(logger, "Assert " #c " error : " __VA_ARGS__))

#endif //AFUN_LOG_H
