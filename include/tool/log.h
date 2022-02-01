﻿#ifndef AFUN_LOG_H
#define AFUN_LOG_H

#include <iostream>
#include "aFunToolExport.h"
#include "tool-type.h"
#include "thread"
#include "mutex"
#include "condition_variable"

namespace aFuntool {
#ifndef __pid_t_defined
    typedef int pid_t;
#define __pid_t_defined
#endif

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

    class AFUN_TOOL_EXPORT LogFactory;

    class AFUN_TOOL_EXPORT Logger {
        friend class LogFactory;
    public:
        inline explicit Logger(LogFactory &factor, std::string id, LogLevel level = log_warning, bool exit = true) noexcept;

        int writeTrackLog(const char *file, int line, const char *func, const char *format, ...);
        int writeDebugLog(const char *file, int line, const char *func, const char *format, ...);
        int writeInfoLog(const char *file, int line, const char *func, const char *format, ...);
        int writeWarningLog(const char *file, int line, const char *func, const char *format, ...);
        int writeErrorLog(const char *file, int line, const char *func, const char *format, ...);
        int writeSendErrorLog(const char *file, int line, const char *func, const char *format, ...) noexcept(false);
        int writeFatalErrorLog(const char *file, int line, const char *func, int exit_code, const char *format, ...) noexcept(false);
    private:
        const std::string id_;
        LogLevel level_ = log_debug;
        bool exit_ = true;
        LogFactory &factor_;
    };

    class AFUN_TOOL_EXPORT LogFactory {
    public:
        Logger sys_log = Logger(*this, "SYSTEM");
        LogFactory();
        ~LogFactory();
        LogFactory(const LogFactory &)=delete;
        LogFactory &operator=(const LogFactory &)=delete;

        int initLogSystem(const aFuntool::FilePath &path, bool is_asyn = true);
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
        struct LogNode *pop();

        struct ansyData;
        void ansyWritrLog(ansyData *data);
    private:
        bool init_;  // 是否已经初始化
        pid_t pid_;

        FILE *log_;  // 记录文件输出的位置
        FILE *csv_;

        bool asyn_;  // 异步
        std::thread thread_;
        std::condition_variable cond_;  // 有日志
        std::mutex mutex_;
        struct LogNode *log_buf_;
        struct LogNode **plog_buf_;  // 指向 log_buf的末端
    };

    struct LogFactory::ansyData {
        LogFactory &factor;
        std::mutex &mutex;
    };
}

#include "log.inline.h"

#ifndef NO_DEFINE_LOG_MACRO
#include "log-macro.h"

#if aFunWriteTrack
#define trackLog(logger, ...) (logger)->writeTrackLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define trackLog(logger, ...) (nullptr)
#endif

#if aFunWriteDebug
#define debugLog(logger, ...) (logger)->writeDebugLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertDebugLog(c, logger, ...) ((c) || debugLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define debugLog(logger, ...) (nullptr)
#define assertDebugLog(c, logger, ...) (c)
#endif

#if aFunWriteInfo
#define infoLog(logger, ...) (logger)->writeInfoLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertInfoLog(c, logger, ...) ((c) || infoLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define infoLog(logger, ...) (nullptr)
#define assertInfoLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreWarning
#define warningLog(logger, ...) (logger)->writeWarningLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertWarningLog(c, logger, ...) ((c) || warningLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define warningLog(logger, ...) (nullptr)
#define assertWarningLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreError
#define errorLog(logger, ...) (logger)->writeErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define assertErrorLog(c, logger, ...) ((c) || errorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define errorLog(logger, ...) (nullptr)
#define assertErrorLog(c, logger, ...) (c)
#endif

#if !aFunOFFAllLog
#define sendErrorLog(logger, ...) (logger)->writeSendErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__)
#define fatalErrorLog(logger, exit_code, ...) (logger)->writeFatalErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, exit_code, __VA_ARGS__)
#define assertSendErrorLog(c, logger, ...) ((c) || sendErrorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertFatalErrorLog(c, logger, exit_code, ...) ((c) || fatalErrorLog(logger, exit_code, "Assert " #c " error : " __VA_ARGS__))
#else
#define sendErrorLog(logger, ...) (nullptr)
#define fatalErrorLog(logger, exit_code, ...) (nullptr)
#define assertSendErrorLog(c, logger, ...) (c)
#define assertFatalErrorLog(c, logger, exit_code, ...) (c)
#endif

#endif
#endif //AFUN_LOG_H
