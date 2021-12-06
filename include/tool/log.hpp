#ifndef AFUN_LOG_HPP
#define AFUN_LOG_HPP
#include <csetjmp>
#include "aFunToolExport.h"
#include "macro.hpp"
#include "log_macro.hpp"

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
    const char *id;
    LogLevel level;
    jmp_buf *buf;
};
typedef struct Logger Logger;

AFUN_TOOL_EXPORT void printLogSystemInfo();
AFUN_TOOL_EXPORT int initLogSystem(FilePath path, bool asyn);
AFUN_TOOL_EXPORT int destructLogSystem();

AFUN_TOOL_EXPORT void initLogger(Logger *logger, const char *id, LogLevel level);
AFUN_TOOL_EXPORT void destructLogger(Logger *logger);

AFUN_TOOL_EXPORT int writeTrackLog_(Logger *logger,
                                    const char *file, int line, const char *func,
                                    const char *format, ...);
AFUN_TOOL_EXPORT int writeDebugLog_(Logger *logger,
                                    const char *file, int line, const char *func,
                                    const char *format, ...);
AFUN_TOOL_EXPORT int writeInfoLog_(Logger *logger,
                                   const char *file, int line, const char *func,
                                   const char *format, ...);
AFUN_TOOL_EXPORT int writeWarningLog_(Logger *logger,
                                      const char *file, int line, const char *func,
                                      const char *format, ...);
AFUN_TOOL_EXPORT int writeErrorLog_(Logger *logger,
                                    const char *file, int line, const char *func,
                                    const char *format, ...);
AFUN_TOOL_EXPORT int writeSendErrorLog_(Logger *logger,
                                        const char *file, int line, const char *func,
                                        const char *format, ...);
AFUN_TOOL_EXPORT int writeFatalErrorLog_(Logger *logger,
                                         const char *file, int line, const char *func,
                                         int exit_code, const char *format, ...);

#ifdef __FILENAME__
#define file_line (char *)__FILENAME__ , (int)__LINE__
#else
#define file_line (char *)"xx", (int)__LINE__
#endif

#if aFunWriteTrack
#define writeTrackLog(logger, ...) writeTrackLog_(logger, file_line, __FUNCTION__, __VA_ARGS__)
#else
#define writeTrackLog(logger, ...) (nullptr)
#endif

#if aFunWriteDebug
#define writeDebugLog(logger, ...) writeDebugLog_(logger, file_line, __FUNCTION__, __VA_ARGS__)
#define assertDebugLog(c, logger, ...) ((c) || writeDebugLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define writeDebugLog(logger, ...) (nullptr)
#define assertDebugLog(c, logger, ...) (c)
#endif

#if aFunWriteInfo
#define writeInfoLog(logger, ...) writeInfoLog_(logger, file_line, __FUNCTION__, __VA_ARGS__)
#define assertInfoLog(c, logger, ...) ((c) || writeInfoLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define writeInfoLog(logger, ...) (nullptr)
#define assertInfoLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreWarning
#define writeWarningLog(logger, ...) writeWarningLog_(logger, file_line, __FUNCTION__, __VA_ARGS__)
#define assertWarningLog(c, logger, ...) ((c) || writeWarningLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define writeWarningLog(logger, ...) (nullptr)
#define assertWarningLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreError
#define writeErrorLog(logger, ...) writeErrorLog_(logger, file_line, __FUNCTION__, __VA_ARGS__)
#define assertErrorLog(c, logger, ...) ((c) || writeErrorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define writeErrorLog(logger, ...) (nullptr)
#define assertErrorLog(c, logger, ...) (c)
#endif

#if !aFunOFFAllLog
#define writeSendErrorLog(logger, ...) writeSendErrorLog_(logger, file_line, __FUNCTION__, __VA_ARGS__)
#define writeFatalErrorLog(logger, exit_code, ...) writeFatalErrorLog_(logger, file_line, __FUNCTION__, exit_code, __VA_ARGS__)
#define assertSendErrorLog(c, logger, ...) ((c) || writeSendErrorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertFatalErrorLog(c, logger, exit_code, ...) ((c) || writeFatalErrorLog(logger, exit_code, "Assert " #c " error : " __VA_ARGS__))
#else
#define writeSendErrorLog(logger, ...) (nullptr)
#define writeFatalErrorLog(logger, exit_code, ...) (nullptr)
#define assertSendErrorLog(c, logger, ...) (c)
#define assertFatalErrorLog(c, logger, exit_code, ...) (c)
#endif

#endif //AFUN_LOG_HPP
