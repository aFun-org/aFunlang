/*
 * 文件名: log.c
 * 目标: 日志系统对aFun的API
 * 注意: 因为tool模块需要使用log系统, 因此log系统尽量少点依赖tool模块, 避免造成死循环
 * 仅依赖:
 * time_s.h 中 getTime -> strCopy
 * mem.h 中 free
 * file.h 中 getFileSize
 * stdio_.h
 * str.h
 * exit_.h
 */

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include "tool-type.h"
#include "log.h"
#include "tool-exception.h"
#include "log-macro.h"
#include "tool-time.h"
#include "file.h"
#include "tool-stdio.h"
#include "str.h"
#include "tool-exit.h"

#ifdef aFunWIN32

#include <Windows.h>

#define getpid() static_cast<long>(GetCurrentProcessId())
#define gettid() static_cast<long>(GetCurrentThreadId())
// cygwin没有syscall.h, 因此需要依赖 windows 的 api
#else
#include <unistd.h>
#include "sys/syscall.h"
#define gettid() static_cast<long>(syscall(SYS_gettid))
#define getpid() static_cast<long>(getpid())
#endif

namespace aFuntool {
    typedef struct LogNode LogNode;
    struct LogNode {  // 日志信息记录节点
        LogLevel level = log_info;
        const char *id = "SYSTEM";
        pid_t tid = 0;
        char *date = nullptr;  // 需要释放
        time_t time = 0;
        const char *file = "unknown";
        int line = 1;
        const char *func = "unknown";
        char *info = nullptr;  // 需要释放

        LogNode *next = nullptr;
    };

    void staticAnsyWritrLog(LogFactory::ansyData *data);

    LogFactory::LogFactory(const aFuntool::FilePath &path, bool is_async) noexcept(false)
        : sys_log{*this, "SYSTEM", log_info}{
        std::unique_lock<std::mutex> ul{mutex_};

        char log_path[218] = {0};
        char csv_path[218] = {0};
        pid_ = getpid();  // 获取进程ID

        char *ti = getTime(nullptr, (char *) "%Y-%m-%d%z");
        snprintf(log_path, 218, "%s-%s.log", path.c_str(), ti);
        snprintf(csv_path, 218, "%s-%s.csv", path.c_str(), ti);
        safeFree(ti);

        uintmax_t log_size = getFileSize(log_path);
        uintmax_t csv_size = getFileSize(csv_path);
        bool csv_head_write = (checkFile(csv_path) == 0);  // 文件不存在时才写入头部

        log_ = fileOpen(log_path, "a");
        if (log_ == nullptr)
            throw FileOpenException(log_path);

        csv_ = fileOpen(csv_path, (char *) "a");
        if (csv_ == nullptr)
            throw FileOpenException(csv_path);

#define CSV_FORMAT "%s,%s,%d,%d,%s,%lld,%s,%d,%s,%s\n"
#define CSV_TITLE  "Level,Logger,PID,TID,Data,Timestamp,File,Line,Function,Log\n"
        if (csv_head_write) {
            fprintf(csv_, CSV_TITLE);  // 设置 cvs 标题
            fflush(csv_);
        }
#undef CSV_TITLE

        init_ = true;
        async_ = is_async;
        if (is_async) {
            log_buf_ = nullptr;
            plog_buf_ = &log_buf_;
            auto *data = new ansyData{*this, mutex_};
            thread_ = std::thread(staticAnsyWritrLog, data);
        }

        ul.unlock();
        infoLog(&this->sys_log, "Log system init success");
        infoLog(&this->sys_log, "Log .log size %lld", log_size);
        infoLog(&this->sys_log, "Log .csv size %lld", csv_size);
    }

    LogFactory::~LogFactory(){
        std::unique_lock<std::mutex> ul{mutex_};

        ul.unlock();
        infoLog(&this->sys_log, "Log system destruct by exit.");  // 需要用锁

        ul.lock();
        init_ = false;
        if (async_) {
            ul.unlock();
            cond_.notify_all();
            thread_.join();
            ul.lock();
            if (log_buf_ != nullptr)
                printf_stderr(0, "Logsystem destruct error.");
        }

        fileClose(log_);
        fileClose(csv_);
        log_ = nullptr;
        csv_ = nullptr;
    }

    static const char *LogLevelName[] = {
            /* LogLevel和字符串的转换 */
            "TK",  // track 0
            "DE",  // debug 1
            "IN",  // info 2
            "WA",  // warning 3
            "ER",  // error 4
            "SE",  // send_error 5
            "FE",  // fatal_error 6
    };

    static const char *LogLevelNameLong[] = {
            /* 内容输出到终端时使用*/
            "Track",  // track 0
            "Debug",  // debug 1
            "Info",  // info 2
            "Warning",  // warning 3
            "Error",  // error 4
            "Fatal Error",  // send_error 5
            "*FATAL ERROR*",  // fatal_error 6
    };

/**
 * 日志写入到文件
 * @param level 日志等级
 * @param id 日志器ID
 * @param tid 线程号
 * @param ti 时间
 * @param t 时间戳
 * @param file 文件名
 * @param line 行号
 * @param func 函数名
 * @param info 日志内容
 */
    void LogFactory::writeLog(LogLevel level,
                              const char *id, pid_t tid,
                              const char *ti, time_t t,
                              const char *file, int line, const char *func,
                              const char *info){
#define FORMAT "%s/[%s] %d %d {%s %lld} (%s:%d at %s) : '%s' \n"
        /* 写入文件日志 */
        if (log_ != nullptr) {
            fprintf(log_, FORMAT, LogLevelName[level], id, pid_, tid, ti, static_cast<long long>(t), file, line, func, info);
            fflush(log_);
        }
        if (csv_ != nullptr) {
            fprintf(csv_, CSV_FORMAT, LogLevelName[level], id, pid_, tid, ti, static_cast<long long>(t), file, line, func, info);
            fflush(csv_);
        }

#undef FORMAT
#undef CSV_FORMAT
    }

/**
 * 日志写入到控制台
 * @param level 日志等级
 * @param id 日志器ID
 * @param tid 线程号
 * @param ti 时间
 * @param t 时间戳
 * @param file 文件名
 * @param line 行号
 * @param func 函数名
 * @param info 日志内容
 */
    void LogFactory::writeConsole(LogLevel level,
                                  const char *id, pid_t tid,
                                  const char *ti, time_t t,
                                  const char *file, int line,
                                  const char *info){
        if (level < log_warning) {
            cout << "\r* " << LogLevelNameLong[level] << "/[" << id << "] " << tid;
            cout << " " << t << " " << ti << " (" << file << ":" << line << ") : '" << info << "'\n";
            fflush(stdout);
        } else {
            cerr << "\r* " << LogLevelNameLong[level] << "/[" << id << "] " << tid;
            cerr << " " << t << " " << ti << " (" << file << ":" << line << ") : '" << info << "'\n";
            fflush(stderr);
        }
    }

/**
 * 日志异步写入
 * @param level 日志等级
 * @param id 日志器ID
 * @param tid 线程号
 * @param ti 时间
 * @param t 时间戳
 * @param file 文件名
 * @param line 行号
 * @param func 函数名
 * @param info 日志内容
 */
    void LogFactory::writeLogAsyn(LogLevel level,
                                  const char *id, pid_t tid,
                                  const char *ti, time_t t,
                                  const char *file, int line, const char *func, const char *info){
#define D(i) (*(plog_buf_))->i
        *(plog_buf_) = new LogNode;
        D(level) = level;
        D(id) = id;
        D(tid) = tid;
        D(date) = strCopy(ti);
        D(time) = t;
        D(file) = file;
        D(line) = line;
        D(func) = func;
        D(info) = strCopy(info);
        plog_buf_ = &(D(next));
#undef D
    }


    LogNode *LogFactory::pop(){
        struct LogNode *n = log_buf_;
        if (n != nullptr) {
            log_buf_ = n->next;
            if (log_buf_ == nullptr)
                plog_buf_ = &log_buf_;
        }
        return n;
    }


    void staticAnsyWritrLog(LogFactory::ansyData *data){
        data->factor.ansyWritrLog(data);
    }


/**
 * 异步写入日志程序
 * @return
 */
    void LogFactory::ansyWritrLog(ansyData *data){
        std::unique_lock<std::mutex> ul{mutex_};
        while (true) {
            while (init_ && log_buf_ == nullptr)
                cond_.wait(ul);
            if (!init_ && log_buf_ == nullptr)
                break;

            LogNode *tmp = data->factor.pop();
#define D(i) tmp->i
            data->factor.writeLog(D(level), D(id), D(tid), D(date), D(time), D(file), D(line), D(func), D(info));
#undef D
            safeFree(tmp->date);
            safeFree(tmp->info);
            delete tmp;
        }
        delete data;
    }

/**
 * 日志器 写入日志
 * @param logger 日志器
 * @param pc  是否写入到控制台
 * @param level 日志等级
 * @param file 文件名
 * @param line 行号
 * @param func 函数名
 * @param format 日志内容(格式字符串)
 * @param ap 格式字符串内容
 * @return
 */
    int LogFactory::newLog(Logger *logger,
                           bool pc,
                           LogLevel level,
                           const char *file, int line, const char *func,
                           const char *format, va_list ap){
        if (logger->level_ > level)
            return 2;

        std::unique_lock<std::mutex> ul{mutex_};
        if (!init_ || log_ == nullptr) {
            return 1;
        }
        clear_ferror(log_);

        // 输出 head 信息
        time_t t = 0;
        char *ti = getTime(&t, ("%Y-%m-%d %H:%M:%S"));
        pid_t tid = gettid();

        char tmp[2048] = {0};
        vsnprintf(tmp, 1024, format, ap);  // ap只使用一次
        va_end(ap);

        if (async_)
            writeLogAsyn(level, logger->id_.c_str(), tid, ti, t, file, line, func, tmp);
        else
            writeLog(level, logger->id_.c_str(), tid, ti, t, file, line, func, tmp);

        if (pc)
            writeConsole(level, logger->id_.c_str(), tid, ti, t, file, line, tmp);

        ul.unlock();
        if (async_)
            cond_.notify_all();
        safeFree(ti);
        return 0;
    }

#undef trackLog

    int Logger::writeTrackLog(const char *file, int line, const char *func,
                              const char *format, ...){
#if aFunWriteTrack
        va_list ap;
        va_start(ap, format);
        return factor_.newLog(this, aFunConsoleTrack, log_track, file, line, func, format, ap);
#endif
    }

#undef debugLog

    int Logger::writeDebugLog(const char *file, int line, const char *func,
                              const char *format, ...){
#if aFunWriteDebug
        va_list ap;
        va_start(ap, format);
        return factor_.newLog(this, aFunConsoleDebug, log_debug, file, line, func, format, ap);
#endif
    }

#undef infoLog

    int Logger::writeInfoLog(const char *file, int line, const char *func,
                             const char *format, ...){
#if aFunWriteInfo
        va_list ap;
        va_start(ap, format);
        return factor_.newLog(this, aFunConsoleInfo, log_info, file, line, func, format, ap);
#endif
    }

#undef warningLog

    int Logger::writeWarningLog(const char *file, int line, const char *func,
                                const char *format, ...){
#if !aFunIgnoreWarning
        va_list ap;
        va_start(ap, format);
        return factor_.newLog(this, aFunConsoleWarning, log_warning, file, line, func, format, ap);
#endif
    }

#undef errorLog

    int Logger::writeErrorLog(const char *file, int line, const char *func,
                              const char *format, ...){
#if !aFunIgnoreError
        va_list ap;
        va_start(ap, format);
        return factor_.newLog(this, aFunConsoleError, log_error, file, line, func, format, ap);
#endif
    }

#undef sendErrorLog

    int Logger::writeSendErrorLog(const char *file, int line, const char *func,
                                  const char *format, ...) noexcept(false) {
#ifndef aFunOFFAllLog
#if !aFunIgnoreSendError
        va_list ap;
        va_start(ap, format);
        factor_.newLog(this, aFunConsoleSendError, log_send_error, file, line, func, format, ap);
#endif

        if (this->exit_)
            throw LogFatalError("Log Fatal Error");
        aFunExit(EXIT_FAILURE);
#endif
        return 0;
    }

#undef fatalErrorLog

    int Logger::writeFatalErrorLog(const char *file, int line, const char *func,
                                   int exit_code, const char *format, ...) noexcept(false){
#ifndef aFunOFFAllLog
#if !aFunIgnoreFatal
        va_list ap;
        va_start(ap, format);
        this->factor_.newLog(this, aFunConsoleFatalError, log_fatal_error, file, line, func, format, ap);
#endif
        if (exit_code == EXIT_SUCCESS)
            abort();
        else
            aFunExit(exit_code);
#endif
        return 0;
    }
}