/*
 * 文件名: log.c
 * 目标: 日志系统对aFun的API
 * 注意: 因为tool模块需要使用log系统, 因此log系统尽量少点依赖tool模块, 避免造成死循环
 * 仅依赖:
 * time_s.h 中 getTime
 * mem.h 中 free
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "macro.h"
#include "mem.h"
#include "log.h"
#include "time_s.h"

#if aFunWIN32
#include <windows.h>
#define getpid() (long)GetCurrentProcessId()
#define gettid() (long)GetCurrentThreadId()
#else
#include <unistd.h>
#include "sys/syscall.h"
#define gettid() (long)syscall(SYS_gettid)
#define getpid() (long)getpid()
#endif

#undef calloc

static struct LogFactory {
    bool init;  // 是否已经初始化

    FILE *log;  // 记录文件输出的位置
    LogFactoryPrintConsole print_console;  // 输出到终端、

    Logger sys_log;
} log_factory = {.init=false};

static void destructLogSystem_at_exit(void);

/*
 * 函数名: initLogSystem
 * 目标: 初始化日志系统
 * 返回值:
 * 1 表示初始化成功
 * 2 表示已经初始化
 * 0 表示初始化失败
 */
int initLogSystem(FilePath path, LogFactoryPrintConsole print_console) {
    if (log_factory.init)
        return 2;
    if (strlen(path) >= 218)  // 路径过长
        return 0;

    char log_path[1024] = {0};
    char log_base[1024] = {0};
    long pid = getpid();  // 获取进程ID

    time_t t;
    char *ti = getTime(&t);

    snprintf(log_path, 1024, "%s%ld-%ld.log", path, pid, t);
    snprintf(log_base, 1024, "%s-base.log", path);

    FILE *base = fopen(log_base, "a");
    if (base == NULL)
        base = fopen(log_path, "w");
    if (base == NULL) {
        free(ti);
        return 0;
    }

    fprintf(base, "%s  %s\n", ti, log_path);
    fclose(base);
    free(ti);

    log_factory.log = fopen(log_path, "a");
    if (log_factory.log == NULL)
        log_factory.log = fopen(log_path, "w");
    if (log_factory.log == NULL)
        return 0;
    log_factory.print_console = print_console;
    log_factory.init = true;
    atexit(destructLogSystem_at_exit);

    initLogger(&(log_factory.sys_log), "SYSTEM", log_debug);  // 设置为 debug, 记录 success 信息
    log_factory.sys_log.process_fatal_error = true;
    log_factory.sys_log.process_send_error = false;
    writeInfoLog(NULL, "Log system init success.");
    log_factory.sys_log.level = log_error;
    return 1;
}

static void destructLogSystem_at_exit(void) {
    if (!log_factory.init)
        return;
    fclose(log_factory.log);
    log_factory.log = NULL;
    log_factory.init = false;
}

int destructLogSystem(void) {
    if (!log_factory.init)
        return 2;
    fclose(log_factory.log);
    log_factory.log = NULL;
    log_factory.init = false;
    return 1;
}

void initLogger(Logger *logger, char *id, LogLevel level) {
    memset(logger, 0, sizeof(Logger));
    logger->id = id;
    logger->level = level;
}

/* LogLevel和字符串的转换 */
static const char *LogLevelName[] = {
        "DE",  // debug 0
        "IN",  // info 1
        "WA",  // warning 2
        "ER",  // error 3
        "SE",  // send_error 4
        "FE",  // fatal_error 5
};

static const char *LogLevelNameLong[] = {
        /* 内容输出到终端时使用*/
        "Debug",  // debug 0
        "Info",  // info 1
        "Warning",  // warning 2
        "Error",  // error 3
        "Fatal Error",  // send_error 4
        "*FATAL ERROR*",  // fatal_error 5
};

static int writeLog_(Logger *logger, LogLevel level, char *file, int line, char *func, char *format, va_list ap){
    if (logger->level > level)
        return 2;
    if (!log_factory.init || log_factory.log == NULL)
        return 1;
    if (ferror(log_factory.log))
        clearerr(log_factory.log);

    // 输出 head 信息
    time_t t = 0;
    char *ti = getTime(&t);

#define FORMAT "%s/[%s] %ld {%s %ld} (%s:%d at %s) : '%s'\n"
#define FORMAT_SHORT "%s[%s] : %s\n"
    long tid = gettid();

    char tmp[2048] = {0};
    vsnprintf(tmp, 1024, format, ap);  // ap只使用一次
    va_end(ap);

    /* 写入文件日志 */
    if (log_factory.log != NULL) {
        fprintf(log_factory.log, FORMAT, LogLevelName[level], logger->id, tid, ti, t, file, line, func, tmp);
        fflush(log_factory.log);
    }

    switch (log_factory.print_console) {
        case log_pc_all:
            if (level < log_warning) {
                fprintf(stdout, FORMAT, LogLevelNameLong[level], logger->id, tid, ti, t, file, line, func, tmp);
                fflush(stdout);
            } else if (log_factory.print_console) {
                fprintf(stderr, FORMAT, LogLevelNameLong[level], logger->id, tid, ti, t, file, line, func, tmp);
                fflush(stderr);
            }
            break;

        case log_pc_w:
            if (level >= log_warning) { // warning的内容一定会被打印
                fprintf(stderr, FORMAT_SHORT, LogLevelNameLong[level], logger->id, tmp);
                fflush(stderr);
            }
            break;

        case log_pc_e:
            if (level >= log_error) {  // warning的内容一定会被打印
                fprintf(stderr, FORMAT_SHORT, LogLevelNameLong[level], logger->id, tmp);
                fflush(stderr);
            }
            break;

        case log_pc_quite:
        default:
            break;
    }

    free(ti);
#undef FORMAT
#undef FORMAT_SHORT
    return 0;
}

#define CHECK_LOGGER() do {if (logger == NULL) {logger = &(log_factory.sys_log);} \
                           if (logger == NULL || logger->id == NULL) return -1;} while(0)
int writeDebugLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, log_debug, file, line, func, format, ap);
}

int writeInfoLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, log_info, file, line, func, format, ap);
}

int writeWarningLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, log_warning, file, line, func, format, ap);
}

int writeErrorLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, log_error, file, line, func, format, ap);
}

int writeSendErrorLog_(Logger *logger, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    int re = writeLog_(logger, log_send_error, file, line, func, format, ap);
    if (logger->process_send_error) {
        jmp_buf *buf = logger->buf;
        initLogger(logger, NULL, 0);  // 清零
        longjmp(*buf, 1);
    }
    return re;
}

int writeFatalErrorLog_(Logger *logger, char *file, int line, char *func, int exit_code, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    int re = writeLog_(logger, log_fatal_error, file, line, func, format, ap);
    if (logger->process_fatal_error)
        exit(exit_code);
    return re;
}
