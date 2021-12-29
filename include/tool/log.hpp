#ifndef AFUN_LOG_HPP
#define AFUN_LOG_HPP

#include <iostream>
#include "aFunToolExport.h"
#include "macro.hpp"
#include "log-m.hpp"

using namespace aFuntool;

namespace aFuntool {
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

    AFUN_TOOL_EXPORT class LogFactory;

    AFUN_TOOL_EXPORT class Logger {
        const std::string id;
        LogLevel level = log_debug;
        bool exit = true;
        friend class LogFactory;
    public:
        explicit Logger(const std::string &id_, LogLevel level_ = log_warning, bool exit_ = true);
        int writeTrackLog(const char *file, int line, const char *func, const char *format, ...);
        int writeDebugLog(const char *file, int line, const char *func, const char *format, ...);
        int writeInfoLog(const char *file, int line, const char *func, const char *format, ...);
        int writeWarningLog(const char *file, int line, const char *func, const char *format, ...);
        int writeErrorLog(const char *file, int line, const char *func, const char *format, ...);
        int writeSendErrorLog(const char *file, int line, const char *func, const char *format, ...);
        int writeFatalErrorLog(const char *file, int line, const char *func, int exit_code, const char *format, ...);
    };


    AFUN_TOOL_EXPORT class LogFactory {
        bool init;  // 是否已经初始化
        pid_t pid;

        FILE *log;  // 记录文件输出的位置
        FILE *csv;

        bool asyn;  // 异步
        pthread_t pt;
        pthread_cond_t cond;  // 有日志
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        struct LogNode *log_buf;
        struct LogNode **plog_buf;  // 指向 log_buf的末端

    public:
        Logger sys_log = Logger("SYSTEM");
        LogFactory();
        ~LogFactory();
        LogFactory(const LogFactory &)=delete;
        LogFactory &operator=(const LogFactory &)=delete;

        int initLogSystem(ConstFilePath path, bool is_asyn = true);
        bool destruct();
        void writeLog(LogLevel level,
                      const char *id, pid_t tid,
                      const char *ti, time_t t,
                      const char *file, int line, const char *func,
                      const char *info);
        static void writeConsole(LogLevel level,
                                 const char *id, pid_t tid,
                                 const char *ti, time_t t,
                                 const char *file, int line, const char *func,
                                 const char *info);
        void writeLogAsyn(LogLevel level,
                          const char *id, pid_t tid,
                          const char *ti, time_t t,
                          const char *file, int line, const char *func, const char *info);
        int newLog(Logger *logger,
                   bool pc,
                   LogLevel level,
                   const char *file, int line, const char *func,
                   const char *format, va_list ap);
        bool news(){ return !init || log_buf != nullptr; }
        int wait(){ return pthread_cond_wait(&cond, &mutex); }
        bool stop(){ return !init && log_buf == nullptr; }
        struct LogNode *pop();
    };

    AFUN_TOOL_EXPORT extern LogFactory log_factory;
}

#ifndef NO_DEFINE_LOG_MACRO

#define getLogger(logger) ((logger) == nullptr ? &aFuntool::log_factory.sys_log : (logger))

#if aFunWriteTrack
#define trackLog(logger, ...) getLogger(logger)->writeTrackLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define trackLog(logger, ...) (nullptr)
#endif

#if aFunWriteDebug
#define debugLog(logger, ...) getLogger(logger)->writeDebugLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertDebugLog(c, logger, ...) ((c) || debugLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define debugLog(logger, ...) (nullptr)
#define assertDebugLog(c, logger, ...) (c)
#endif

#if aFunWriteInfo
#define infoLog(logger, ...) getLogger(logger)->writeInfoLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertInfoLog(c, logger, ...) ((c) || infoLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define infoLog(logger, ...) (nullptr)
#define assertInfoLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreWarning
#define warningLog(logger, ...) getLogger(logger)->writeWarningLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertWarningLog(c, logger, ...) ((c) || warningLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define warningLog(logger, ...) (nullptr)
#define assertWarningLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreError
#define errorLog(logger, ...) getLogger(logger)->writeErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertErrorLog(c, logger, ...) ((c) || errorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define errorLog(logger, ...) (nullptr)
#define assertErrorLog(c, logger, ...) (c)
#endif

#if !aFunOFFAllLog
#define sendErrorLog(logger, ...) getLogger(logger)->writeSendErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define fatalErrorLog(logger, exit_code, ...) getLogger(logger)->writeFatalErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, exit_code, __VA_ARGS__)
#define assertSendErrorLog(c, logger, ...) ((c) || sendErrorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertFatalErrorLog(c, logger, exit_code, ...) ((c) || fatalErrorLog(logger, exit_code, "Assert " #c " error : " __VA_ARGS__))
#else
#define sendErrorLog(logger, ...) (nullptr)
#define fatalErrorLog(logger, exit_code, ...) (nullptr)
#define assertSendErrorLog(c, logger, ...) (c)
#define assertFatalErrorLog(c, logger, exit_code, ...) (c)
#endif

#endif
#endif //AFUN_LOG_HPP
