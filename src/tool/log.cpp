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
 * pthread.h
 */

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include "macro.h"
#include "log.h"
#include "exception.h"
#include "log-m.h"
#include "time_.h"
#include "file.h"
#include "stdio_.h"
#include "str.h"
#include "exit_.h"
#include "pthread.h"

using namespace aFuntool;

#ifdef aFunWIN32
#include <windows.h>
#define getpid() (long)GetCurrentProcessId()
#define gettid() (long)GetCurrentThreadId()
// cygwin没有syscall.h, 因此需要依赖 windows 的 api
#else
#include <unistd.h>
#include "sys/syscall.h"
#define gettid() (long)syscall(SYS_gettid)
#define getpid() (long)getpid()
#endif

#undef calloc

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

    LogFactory log_factory {};  // NOLINT
}

struct ansyData {
    pthread_mutex_t *mutex;
};

static void destructLogSystemAtExit(void *);
static void *ansyWritrLog(void *);

aFuntool::LogFactory::LogFactory() : sys_log("SYSTEM", log_info) {  // NOLINT cond 通过 pthread_cond_init 实现初始化
    init=false;
    pid=0;
    log = nullptr;
    csv = nullptr;

    asyn=false;
    pthread_cond_init(&cond, nullptr);
    log_buf = nullptr;
    plog_buf = nullptr;
}

/**
 * 函数名: initLogSystem
 * 目标: 初始化日志系统
 * 返回值:
 * 1 表示初始化成功
 * 2 表示已经初始化
 * 0 表示初始化失败
 *
 * 该程序线程不安全
 * @param path 日志保存的地址
 * @param is_asyn 是否启用异步
 * @return
 */
int aFuntool::LogFactory::initLogSystem(ConstFilePath path, bool is_asyn){
    if (path.size() >= 218)  // 路径过长
        return 0;

    int re = 1;
    pthread_mutex_lock(&mutex);
    if (init) {
        pthread_mutex_unlock(&mutex);
        return 2;
    }

    char log_path[218] = {0};
    char csv_path[218] = {0};
    pid = getpid();  // 获取进程ID

    char *ti = getTime(nullptr, (char *)"%Y-%m-%d%z");
    snprintf(log_path, 218, "%s-%s.log", path.c_str(), ti);
    snprintf(csv_path, 218, "%s-%s.csv", path.c_str(), ti);
    free(ti);

    uintmax_t log_size = getFileSize(log_path);
    uintmax_t csv_size = getFileSize(csv_path);
    bool csv_head_write = (checkFile(csv_path) == 0);  // 文件不存在时才写入头部

    log = fileOpen(log_path, "a");
    if (log == nullptr) {
        perror("ERROR: ");
        pthread_mutex_unlock(&mutex);
        return 0;
    }

    csv = fileOpen(csv_path, (char *)"a");
    if (csv == nullptr) {
        pthread_mutex_unlock(&mutex);
        return 0;
    }

#define CSV_FORMAT "%s,%s,%d,%d,%s,%ld,%s,%d,%s,%s\n"
#define CSV_TITLE  "Level,Logger,PID,TID,Data,Timestamp,File,Line,Function,Log\n"
    if (csv_head_write) {
        fprintf(csv, CSV_TITLE);  // 设置 cvs 标题
        fflush(csv);
    }
#undef CSV_TITLE

    init = true;
    asyn = is_asyn;
    if (is_asyn) {
        plog_buf = &log_buf;
        pthread_cond_init(&cond, nullptr);

        auto *data = new ansyData;
        data->mutex = &mutex;
        pthread_create(&pt, nullptr, ansyWritrLog, data);
    }

    pthread_mutex_unlock(&mutex);
    infoLog(nullptr, "Log system init success");
    infoLog(nullptr, "Log .log size %lld", log_size);
    infoLog(nullptr, "Log .csv size %lld", csv_size);
    aFunAtExit(destructLogSystemAtExit, nullptr);
    return re;
}

static void destructLogSystemAtExit(void *) {
    log_factory.destruct();
}

aFuntool::LogFactory::~LogFactory(){
    destruct();
}

bool aFuntool::LogFactory::destruct() {
    bool re = true;
    pthread_mutex_lock(&mutex);
    if (!init) {
        re = false;
        goto RETURN;
    }

    pthread_mutex_unlock(&mutex);

    infoLog(nullptr, "Log system destruct by exit.");  // 需要用锁

    pthread_mutex_lock(&mutex);
    init = false;
    if (asyn) {
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);

        pthread_join(pt, nullptr);

        pthread_mutex_lock(&mutex);
        pthread_cond_destroy(&cond);
        if (log_buf != nullptr)
            printf_stderr(0, "Logsystem destruct error.");
    }

    fileClose(log);
    fileClose(csv);
    log = nullptr;
    csv = nullptr;
RETURN:
    pthread_mutex_unlock(&mutex);
    return re;
}


/* LogLevel和字符串的转换 */
static const char *LogLevelName[] = {
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
void aFuntool::LogFactory::writeLog(LogLevel level,
                               const char *id, pid_t tid,
                               const char *ti, time_t t,
                               const char *file, int line, const char *func,
                               const char *info) {
#define FORMAT "%s/[%s] %d %d {%s %ld} (%s:%d at %s) : '%s' \n"
    /* 写入文件日志 */
    if (log != nullptr) {
        fprintf(log, FORMAT, LogLevelName[level], id, pid, tid, ti, t, file, line, func, info);
        fflush(log);
    }
    if (csv != nullptr) {
        fprintf(csv, CSV_FORMAT, LogLevelName[level], id, pid, tid, ti, t, file, line, func, info);
        fflush(csv);
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
void aFuntool::LogFactory::writeConsole(LogLevel level,
                                        const char *id, pid_t tid,
                                        const char *ti, time_t t,
                                        const char *file, int line, const char *func,
                                        const char *info) {
#define FORMAT_SHORT "\r* %s[%s] %d %s %ld (%s:%d) : %s \n"  // 显示到终端, 添加\r回车符确保顶行显示
#define STD_BUF_SIZE (strlen(info) + 1024)
    if (level < log_warning) {
        printf_stdout(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], id, tid, ti, t, file, line, info);
        fflush(stdout);
    } else {
        printf_stderr(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], id, tid, ti, t, file, line, info);
        fflush(stderr);
    }
#undef FORMAT_SHORT
#undef STD_BUF_SIZE
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
void aFuntool::LogFactory::writeLogAsyn(LogLevel level,
                                        const char *id, pid_t tid,
                                        const char *ti, time_t t,
                                        const char *file, int line, const char *func, const char *info) {
#define D(i) (*(plog_buf))->i
    *(plog_buf) = new aFuntool::LogNode;
    D(level) = level;
    D(id) = id;
    D(tid) = tid;
    D(date) = strCopy(ti);
    D(time) = t;
    D(file) = file;
    D(line) = line;
    D(func) = func;
    D(info) = strCopy(info);
    log_factory.plog_buf = &(D(next));
#undef D
}


aFuntool::LogNode *aFuntool::LogFactory::pop(){
    struct LogNode *n = log_buf;
    if (n != nullptr) {
        log_buf = n->next;
        if (log_buf == nullptr)
            plog_buf = &log_buf;
    }
    return n;
}


/**
 * 异步写入日志程序
 * @return
 */
static void *ansyWritrLog(void *d) {
    auto *data = (struct ansyData *)d;
    pthread_mutex_lock(data->mutex);
    while (true) {
        while (!log_factory.news())
            log_factory.wait();
        if (log_factory.stop())
            break;

        LogNode *tmp = log_factory.pop();
#define D(i) tmp->i
        log_factory.writeLog(D(level), D(id), D(tid), D(date), D(time), D(file), D(line), D(func), D(info));
#undef D
        free(tmp->date);
        free(tmp->info);
        delete tmp;
    }
    pthread_mutex_unlock(data->mutex);
    delete data;
    return nullptr;
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
int aFuntool::LogFactory::newLog(Logger *logger,
                                 bool pc,
                                 LogLevel level,
                                 const char *file, int line, const char *func,
                                 const char *format, va_list ap){
    if (logger->level > level)
        return 2;

    pthread_mutex_lock(&mutex);
    if (!init || log == nullptr) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    clear_ferror(log);

    // 输出 head 信息
    time_t t = 0;
    char *ti = getTime(&t, ("%Y-%m-%d %H:%M:%S"));
    pid_t tid = gettid();

    char tmp[2048] = {0};
    vsnprintf(tmp, 1024, format, ap);  // ap只使用一次
    va_end(ap);

    if (asyn)
        writeLogAsyn(level, logger->id.c_str(), tid, ti, t, file, line, func, tmp);
    else
        writeLog(level, logger->id.c_str(), tid, ti, t, file, line, func, tmp);

    if (pc)
        writeConsole(level, logger->id.c_str(), tid, ti, t, file, line, func, tmp);

    pthread_mutex_unlock(&mutex);
    if (asyn)
        pthread_cond_signal(&cond);
    free(ti);
    return 0;
}

#define CHECK_LOGGER() do {if (logger == nullptr) {logger = &(log_factory.sys_log);} \
                           if (logger == nullptr) return -1;} while(0)

aFuntool::Logger::Logger(const std::string &id_, LogLevel level_, bool exit_) : id {id_} {
    this->level = level_;
    this->exit = exit_;
}

#undef trackLog
int aFuntool::Logger::writeTrackLog(const char *file, int line, const char *func,
                                    const char *format, ...) {
#if aFunWriteTrack
    va_list ap;
    va_start(ap, format);
    return log_factory.newLog(this, aFunConsoleTrack, log_track, file, line, func, format, ap);
#endif
}

#undef debugLog
int aFuntool::Logger::writeDebugLog(const char *file, int line, const char *func,
                                    const char *format, ...) {
#if aFunWriteDebug
    va_list ap;
    va_start(ap, format);
    return log_factory.newLog(this, aFunConsoleDebug, log_debug, file, line, func, format, ap);
#endif
}

#undef infoLog
int aFuntool::Logger::writeInfoLog(const char *file, int line, const char *func,
                                   const char *format, ...) {
#if aFunWriteInfo
    va_list ap;
    va_start(ap, format);
    return log_factory.newLog(this, aFunConsoleInfo, log_info, file, line, func, format, ap);
#endif
}

#undef warningLog
int aFuntool::Logger::writeWarningLog(const char *file, int line, const char *func,
                                      const char *format, ...) {
#if !aFunIgnoreWarning
    va_list ap;
    va_start(ap, format);
    return log_factory.newLog(this, aFunConsoleWarning, log_warning, file, line, func, format, ap);
#endif
}

#undef errorLog
int aFuntool::Logger::writeErrorLog(const char *file, int line, const char *func,
                                    const char *format, ...) {
#if !aFunIgnoreError
    va_list ap;
    va_start(ap, format);
    return log_factory.newLog(this, aFunConsoleError, log_error, file, line, func, format, ap);
#endif
}

#undef sendErrorLog
int aFuntool::Logger::writeSendErrorLog(const char *file, int line, const char *func,
                                        const char *format, ...) {
#ifndef aFunOFFAllLog
#if !aFunIgnoreSendError
    va_list ap;
    va_start(ap, format);
    log_factory.newLog(this, aFunConsoleSendError, log_send_error, file, line, func, format, ap);
#endif

    if (this->exit)
        throw LogFatalError("Log Fatal Error");
    aFuntool::log_factory.destruct();
    aFunExit(EXIT_FAILURE);
#endif
}

#undef fatalErrorLog
int aFuntool::Logger::writeFatalErrorLog(const char *file, int line, const char *func,
                                         int exit_code, const char *format, ...) {
#ifndef aFunOFFAllLog
#if !aFunIgnoreFatal
    va_list ap;
    va_start(ap, format);
    log_factory.newLog(this, aFunConsoleFatalError, log_fatal_error, file, line, func, format, ap);
#endif

    aFuntool::log_factory.destruct();
    if (exit_code == EXIT_SUCCESS)
        abort();
    else
        aFunExit(exit_code);
#endif
}
