#ifndef AFUN_TOOL_LOG_H
#define AFUN_TOOL_LOG_H
#ifdef __cplusplus

#include <iostream>
#include "aFunToolExport.h"
#include "tool_macro.h"

#include "tool.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

#ifdef _MSC_VER
    typedef int pid_t;
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
        AFUN_INLINE explicit Logger(LogFactory &factor, std::string id, LogLevel level = log_warning, bool exit = true) noexcept;

        int writeTrackLog(const char *file, int line, const char *func, const char *format, ...);
        int writeDebugLog(const char *file, int line, const char *func, const char *format, ...);
        int writeInfoLog(const char *file, int line, const char *func, const char *format, ...);
        int writeWarningLog(const char *file, int line, const char *func, const char *format, ...);
        int writeErrorLog(const char *file, int line, const char *func, const char *format, ...);
        int writeSendErrorLog(const char *file, int line, const char *func, const char *format, ...) noexcept(false);
        int writeFatalErrorLog(const char *file, int line, const char *func, int exit_code, const char *format, ...) noexcept(false);
    private:
        LogFactory &factor_;
        const std::string id_;
        LogLevel level_ = log_debug;
        bool exit_ = true;
    };

    class AFUN_TOOL_EXPORT LogFactory {
    public:
        Logger sys_log = Logger(*this, "SYSTEM");
        LogFactory(const FilePath &path, bool is_async) noexcept(false);
        ~LogFactory();
        LogFactory(const LogFactory &)=delete;
        LogFactory &operator=(const LogFactory &)=delete;

        void writeLog(LogLevel level,
                      const char *id, pid_t tid,
                      const char *ti, time_t t,
                      const char *file, int line, const char *func,
                      const char *info);
        AFUN_STATIC void writeConsole(LogLevel level,
                                 const char *id, pid_t tid,
                                 const char *ti, time_t t,
                                 const char *file, int line,
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

        bool async_;  // 异步
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

#ifndef AFUN_TOOL_C
}
#endif

#include "tool_log.inline.h"

#ifndef NO_DEFINE_LOG_MACRO
#include "tool_log_macro.h"

#if aFunWriteTrack
#define trackLog(logger, ...) ((logger) ? (logger)->writeTrackLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__) : 0)
#else
#define trackLog(logger, ...) (nullptr)
#endif

#if aFunWriteDebug
#define debugLog(logger, ...) ((logger) ? (logger)->writeDebugLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__) : 0)
#define assertDebugLog(c, logger, ...) ((c) || debugLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define debugLog(logger, ...) (nullptr)
#define assertDebugLog(c, logger, ...) (c)
#endif

#if aFunWriteInfo
#define infoLog(logger, ...) ((logger) ? (logger)->writeInfoLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__) : 0)
#define assertInfoLog(c, logger, ...) ((c) || infoLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define infoLog(logger, ...) (nullptr)
#define assertInfoLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreWarning
#define warningLog(logger, ...) ((logger) ? (logger)->writeWarningLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__) : 0)
#define assertWarningLog(c, logger, ...) ((c) || warningLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define warningLog(logger, ...) (nullptr)
#define assertWarningLog(c, logger, ...) (c)
#endif

#if !aFunIgnoreError
#define errorLog(logger, ...) ((logger) ? (logger)->writeErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__) : 0)
#define assertErrorLog(c, logger, ...) ((c) || errorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#else
#define errorLog(logger, ...) (nullptr)
#define assertErrorLog(c, logger, ...) (c)
#endif

#if !aFunOFFAllLog
#define sendErrorLog(logger, ...) ((logger) ? (logger)->writeSendErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, __VA_ARGS__) : 0)
#define fatalErrorLog(logger, exit_code, ...) ((logger) ? (logger)->writeFatalErrorLog(__FILENAME__ , (int)__LINE__, __FUNCTION__, exit_code, __VA_ARGS__) : 0)
#define assertSendErrorLog(c, logger, ...) ((c) || sendErrorLog(logger, "Assert " #c " error : " __VA_ARGS__))
#define assertFatalErrorLog(c, logger, exit_code, ...) ((c) || fatalErrorLog(logger, exit_code, "Assert " #c " error : " __VA_ARGS__))
#else
#define sendErrorLog(logger, ...) (nullptr)
#define fatalErrorLog(logger, exit_code, ...) (nullptr)
#define assertSendErrorLog(c, logger, ...) (c)
#define assertFatalErrorLog(c, logger, exit_code, ...) (c)
#endif

#endif

#endif  // __cplusplus
#endif //AFUN_TOOL_LOG_H
