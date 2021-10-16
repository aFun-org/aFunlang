﻿/*
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "macro.h"
#include "mem.h"
#include "log.h"
#include "time_s.h"
#include "file.h"
#include "stdio_.h"
#include "str.h"
#include "exit_.h"
#include "pthread.h"

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

typedef struct LogNode LogNode;
struct LogNode {
    LogLevel level;
    char *id;
    pid_t tid;
    char *date;  // 需要释放
    time_t time;
    char *file;
    int line;
    char *func;
    char *info;  // 需要释放

    LogNode *next;
};

static struct LogFactory {
    bool init;  // 是否已经初始化
    pid_t pid;

    FILE *log;  // 记录文件输出的位置
    FILE *csv;

    Logger sys_log;

    bool asyn;  // 异步
    pthread_t pt;
    pthread_cond_t cond;  // 有日志
    LogNode *log_buf;
    LogNode **plog_buf;  // 指向 log_buf的末端
} log_factory = {.init=false};
static pthread_mutex_t log_factory_mutex = PTHREAD_MUTEX_INITIALIZER;
#define MUTEX (&log_factory_mutex)

static void destructLogSystemAtExit(void *data);
static void *ansyWritrLog_(void *_);

/*
 * 函数名: initLogSystem
 * 目标: 初始化日志系统
 * 返回值:
 * 1 表示初始化成功
 * 2 表示已经初始化
 * 0 表示初始化失败
 *
 * 该程序线程不安全
 */
int initLogSystem(FilePath path, bool asyn){
    if (strlen(path) >= 218)  // 路径过长
        return 0;

    int re = 1;
    pthread_mutex_lock(MUTEX);
    if (log_factory.init) {
        pthread_mutex_unlock(MUTEX);
        return 2;
    }

    char log_path[218] = {0};
    char csv_path[218] = {0};
    log_factory.pid = getpid();  // 获取进程ID

    char *ti = getTime(NULL, "%Y-%m-%d%z");
    snprintf(log_path, 218, "%s-%s.log", path, ti);
    snprintf(csv_path, 218, "%s-%s.csv", path, ti);
    free(ti);

    uintmax_t log_size = getFileSize(log_path);
    uintmax_t csv_size = getFileSize(csv_path);
    bool csv_head_write = (checkFile(csv_path) == 0);  // 文件不存在时才写入头部

    log_factory.log = fileOpen(log_path, "a");
    if (log_factory.log == NULL) {
        perror("ERROR: ");
        printf("log_path = %s\n", log_path);
        pthread_mutex_unlock(MUTEX);
        return 0;
    }

    log_factory.csv = fileOpen(csv_path, "a");
    if (log_factory.csv == NULL) {
        pthread_mutex_unlock(MUTEX);
        return 0;
    }

#define CSV_FORMAT "%s,%s,%d,%d,%s,%ld,%s,%d,%s,%s\n"
#define CSV_TITLE  "Level,Logger,PID,TID,Data,Timestamp,File,Line,Function,Log\n"
    if (csv_head_write) {
        fprintf(log_factory.csv, CSV_TITLE);  // 设置 cvs 标题
        fflush(log_factory.csv);
    }
#undef CSV_TITLE

    log_factory.init = true;
    log_factory.asyn = asyn;
    if (log_factory.asyn) {
        pthread_cond_init(&log_factory.cond, NULL);
        pthread_create(&log_factory.pt, NULL, ansyWritrLog_, NULL);
    }

    initLogger(&(log_factory.sys_log), "SYSTEM", log_info);  // 设置为 debug, 记录 success 信息
    pthread_mutex_unlock(MUTEX);
    writeInfoLog(NULL, "Log system init success");
    writeInfoLog(NULL, "Log .log size %lld", log_size);
    writeInfoLog(NULL, "Log .csv size %lld", csv_size);
    aFunAtExit(destructLogSystemAtExit, NULL);
    return re;
}

static void destructLogSystemAtExit(void *data) {
    destructLogSystem();
}

int destructLogSystem(void) {
    int re = 1;
    pthread_mutex_lock(MUTEX);
    if (!log_factory.init) {
        re = 2;
        goto RETURN;
    }
    pthread_mutex_unlock(MUTEX);

    writeInfoLog(NULL, "Log system destruct by exit.");  // 需要用锁

    pthread_mutex_lock(MUTEX);
    log_factory.init = false;
    if (log_factory.asyn) {
        pthread_mutex_unlock(MUTEX);
        pthread_cond_signal(&log_factory.cond);

        pthread_join(log_factory.pt, NULL);

        pthread_mutex_lock(MUTEX);
        pthread_cond_destroy(&log_factory.cond);
        if (log_factory.log_buf != NULL)
            printf_stderr(0, "Logsystem destruct error.");
    }

    fileClose(log_factory.log);
    fileClose(log_factory.csv);
    log_factory.log = NULL;
    log_factory.csv = NULL;
    log_factory.init = false;
RETURN:
    pthread_mutex_unlock(MUTEX);
    return re;
}

void initLogger(Logger *logger, char *id, LogLevel level) {
    memset(logger, 0, sizeof(Logger));
    logger->id = id;
    logger->level = level;
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

static void writeLogToFactory_(LogLevel level, char *id, pid_t tid, char *ti, time_t t, char *file, int line, char *func, char *info) {
#define FORMAT "%s/[%s] %d %d {%s %ld} (%s:%d at %s) : '%s' \n"
    /* 写入文件日志 */
    if (log_factory.log != NULL) {
        fprintf(log_factory.log, FORMAT, LogLevelName[level], id, log_factory.pid, tid, ti, t, file, line, func, info);
        fflush(log_factory.log);
    }
    if (log_factory.csv != NULL) {
        fprintf(log_factory.csv, CSV_FORMAT, LogLevelName[level], id, log_factory.pid, tid, ti, t, file, line, func, info);
        fflush(log_factory.csv);
    }

#undef FORMAT
#undef CSV_FORMAT
}

static void writeLogToConsole_(LogLevel level, char *id, pid_t tid, char *ti, time_t t, char *file, int line, char *func, char *info) {
#define FORMAT_SHORT "\r* %s(%s:%d) : %s \n"  // 显示到终端, 添加\r回车符确保顶行显示
#define STD_BUF_SIZE (STR_LEN(info) + 1024)
    if (level < log_warning) {
        printf_stdout(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], file, line, info);
        fflush(stdout);
    } else {
        printf_stderr(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], file, line, info);
        fflush(stderr);
    }
#undef FORMAT_SHORT
#undef STD_BUF_SIZE
}

static void writeLogToAsyn_(LogLevel level, char *id, pid_t tid, char *ti, time_t t, char *file, int line, char *func, char *info) {
#define D(i) (*(log_factory.plog_buf))->i
    *(log_factory.plog_buf) = calloc(1, sizeof(LogNode));
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
    pthread_cond_signal(&log_factory.cond);
#undef D
}

static void *ansyWritrLog_(void *_) {
    pthread_mutex_lock(MUTEX);
    while (1) {
        while (log_factory.init && log_factory.log_buf == NULL)
            pthread_cond_wait(&log_factory.cond, MUTEX);
        if (!log_factory.init && log_factory.log_buf == NULL)
            break;
#define D(i) log_factory.log_buf->i
        writeLogToFactory_(D(level), D(id), D(tid), D(date), D(time), D(file), D(line), D(func), D(info));
#undef D
        LogNode *tmp = log_factory.log_buf->next;
        free(log_factory.log_buf->date);
        free(log_factory.log_buf->info);
        free(log_factory.log_buf);
        log_factory.log_buf = tmp;
        if (tmp == NULL)
            log_factory.plog_buf = &log_factory.log_buf;
    }
    pthread_mutex_unlock(MUTEX);
    return NULL;
}

static int writeLog_(Logger *logger, bool pc, LogLevel level, char *file, int line, char *func, char *format, va_list ap){
    if (logger->level > level)
        return 2;

    pthread_mutex_lock(MUTEX);
    if (!log_factory.init || log_factory.log == NULL) {
        pthread_mutex_unlock(MUTEX);
        return 1;
    }
    CLEAR_FERROR(log_factory.log);

    // 输出 head 信息
    time_t t = 0;
    char *ti = getTime(&t, "%Y-%m-%d %H:%M:%S");
    pid_t tid = gettid();

    char tmp[2048] = {0};
    vsnprintf(tmp, 1024, format, ap);  // ap只使用一次
    va_end(ap);

    if (!log_factory.asyn)
        writeLogToAsyn_(level, logger->id, tid, ti, t, file, line, func, tmp);
    else
        writeLogToFactory_(level, logger->id, tid, ti, t, file, line, func, tmp);

    if (pc)
        writeLogToConsole_(level, logger->id, tid, ti, t, file, line, func, tmp);

    pthread_mutex_unlock(MUTEX);
    free(ti);
    return 0;
}

#define CHECK_LOGGER() do {if (logger == NULL) {logger = &(log_factory.sys_log);} \
                           if (logger == NULL || logger->id == NULL) return -1;} while(0)

int writeTrackLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
#if aFunWriteTrack
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, aFunConsoleTrack, log_track, file, line, func, format, ap);
#endif
}

int writeDebugLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
#if aFunWriteDebug
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, aFunConsoleDebug, log_debug, file, line, func, format, ap);
#endif
}

int writeInfoLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
#if aFunWriteInfo
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, aFunConsoleInfo, log_info, file, line, func, format, ap);
#endif
}

int writeWarningLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
#if !aFunIgnoreWarning
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, aFunConsoleWarning, log_warning, file, line, func, format, ap);
#endif
}

int writeErrorLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
#if !aFunIgnoreError
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, aFunConsoleError, log_error, file, line, func, format, ap);
#endif
}

int writeSendErrorLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
#ifndef aFunOFFAllLog
    CHECK_LOGGER();
#if !aFunIgnoreSendError
    va_list ap;
    va_start(ap, format);
    jmp_buf *buf = logger->buf;
    writeLog_(logger, aFunConsoleSendError, log_send_error, file, line, func, format, ap);
#endif

    destructLogSystem();
    if (buf != NULL) {
        initLogger(logger, NULL, 0);  // 清零
        longjmp(*buf, 1);
    } else
        aFunExit(aFunExitFail);
#endif
}

int writeFatalErrorLog_(Logger *logger, char *file, int line, char *func, int exit_code, char *format, ...) {
#ifndef aFunOFFAllLog
    CHECK_LOGGER();

#if !aFunIgnoreFatal
    va_list ap;
    va_start(ap, format);
    writeLog_(logger, aFunConsoleFatalError, log_fatal_error, file, line, func, format, ap);
#endif

    destructLogSystem();
    if (exit_code == EXIT_SUCCESS)
        abort();
    else
        aFunExit(exit_code);
#endif
}
