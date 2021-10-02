/*
 * 文件名: log.c
 * 目标: 日志系统对aFun的API
 * 注意: 因为tool模块需要使用log系统, 因此log系统尽量少点依赖tool模块, 避免造成死循环
 * 仅依赖:
 * time_s.h 中 getTime -> strCopy
 * mem.h 中 free
 * file.h 中 getFileSize
 * stdio_.h
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

static struct LogFactory {
    bool init;  // 是否已经初始化
    long pid;

    FILE *log;  // 记录文件输出的位置
    FILE *csv;
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

    char log_path[218] = {0};
    char csv_path[218] = {0};
    log_factory.pid = getpid();  // 获取进程ID
    char *ti = getTime(NULL, "%Y-%m-%d%z");

    snprintf(log_path, 218, "%s-%s.log", path, ti);
    snprintf(csv_path, 218, "%s-%s.csv", path, ti);

    uintmax_t log_size = getFileSize(log_path);
    uintmax_t csv_size = getFileSize(csv_path);
    bool csv_head_write = (checkFile(csv_path) == 0);  // 文件不存在时才写入头部

    log_factory.log = fopen(log_path, "a");
    if (log_factory.log == NULL)
        return 0;

    log_factory.csv = fopen(csv_path, "a");
    if (log_factory.csv == NULL)
        return 0;

#define CSV_FORMAT "%s,%s,%ld,%ld,%s,%ld,%s,%d,%s,%s\n"
#define CSV_TITLE  "Level,Logger,PID,TID,Data,Timestamp,File,Line,Function,Log\n"
    if (csv_head_write) {
        fprintf(log_factory.csv, CSV_TITLE);  // 设置 cvs 标题
        fflush(log_factory.csv);
    }
#undef CSV_TITLE

    log_factory.print_console = print_console;
    log_factory.init = true;
    atexit(destructLogSystem_at_exit);

    initLogger(&(log_factory.sys_log), "SYSTEM", log_debug);  // 设置为 debug, 记录 success 信息
    log_factory.sys_log.process_fatal_error = true;
    log_factory.sys_log.process_send_error = false;
    writeInfoLog(NULL, log_default, "Log system init success");
    writeInfoLog(NULL, log_default, "Log .log size %lld", log_size);
    writeInfoLog(NULL, log_default, "Log .csv size %lld", csv_size);
    log_factory.sys_log.level = log_error;
    return 1;
}

static void destructLogSystem_at_exit(void) {
    if (!log_factory.init)
        return;
    log_factory.sys_log.level = log_debug;
    writeInfoLog(NULL, log_default, "Log system init destruct by atexit.");
    fclose(log_factory.log);
    fclose(log_factory.csv);
    log_factory.log = NULL;
    log_factory.csv = NULL;
    log_factory.init = false;
}

int destructLogSystem(void) {
    if (!log_factory.init)
        return 2;
    log_factory.sys_log.level = log_debug;
    writeInfoLog(NULL, log_default, "Log system init destruct by user.");
    fclose(log_factory.log);
    fclose(log_factory.csv);
    log_factory.log = NULL;
    log_factory.csv = NULL;
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

static int writeLog_(Logger *logger, LogLoggerPrintConsole pc, LogLevel level, char *file, int line, char *func, char *format, va_list ap){
    if (logger->level > level)
        return 2;
    if (!log_factory.init || log_factory.log == NULL)
        return 1;
    if (ferror(log_factory.log))
        clearerr(log_factory.log);

    // 输出 head 信息
    time_t t = 0;
    char *ti = getTime(&t, "%Y-%m-%d %H:%M:%S");

#define FORMAT "%s/[%s] %ld %ld {%s %ld} (%s:%d at %s) : '%s'\n"
#define FORMAT_SHORT "%s[%s] (%s:%d at %s) : %s\n"
    long tid = gettid();

    char tmp[2048] = {0};
    vsnprintf(tmp, 1024, format, ap);  // ap只使用一次
    va_end(ap);

    /* 写入文件日志 */
    if (pc != log_print_console && log_factory.log != NULL) {
        fprintf(log_factory.log, FORMAT, LogLevelName[level], logger->id, log_factory.pid, tid, ti, t, file, line, func, tmp);
        fflush(log_factory.log);
    }
    if (pc != log_print_console && log_factory.csv != NULL) {
        fprintf(log_factory.csv, CSV_FORMAT, LogLevelName[level], logger->id, log_factory.pid, tid, ti, t, file, line, func, tmp);
        fflush(log_factory.csv);
    }

#define STD_BUF_SIZE (strlen(tmp) + 1024)
    if (pc != log_print_no_console) {
        switch (log_factory.print_console) {
            case log_pc_all:
                if (level < log_warning) {
                    printf_stdout(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], logger->id, file, line, func, tmp);
                    fflush(stdout);
                } else if (log_factory.print_console) {
                    printf_stderr(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], logger->id, file, line, func, tmp);
                    fflush(stderr);
                }
                break;
            case log_pc_w:
                if (level >= log_warning) { // warning的内容一定会被打印
                    printf_stderr(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], logger->id, file, line, func, tmp);
                    fflush(stderr);
                }
                break;
            case log_pc_e:
                if (level >= log_error) {  // warning的内容一定会被打印
                    printf_stderr(STD_BUF_SIZE, FORMAT_SHORT, LogLevelNameLong[level], logger->id, file, line, func, tmp);
                    fflush(stderr);
                }
                break;

            case log_pc_quite:
            default:
                break;
        }
    }
#undef STD_BUF_SIZE

    free(ti);
#undef FORMAT
#undef FORMAT_SHORT
#undef CSV_FORMAT
    return 0;
}

#define CHECK_LOGGER() do {if (logger == NULL) {logger = &(log_factory.sys_log);} \
                           if (logger == NULL || logger->id == NULL) return -1;} while(0)
int writeDebugLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, pc, log_debug, file, line, func, format, ap);
}

int writeInfoLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, pc, log_info, file, line, func, format, ap);
}

int writeWarningLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, pc, log_warning, file, line, func, format, ap);
}

int writeErrorLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    return writeLog_(logger, pc, log_error, file, line, func, format, ap);
}

int writeSendErrorLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    int re = writeLog_(logger, pc, log_send_error, file, line, func, format, ap);
    if (logger->process_send_error) {
        jmp_buf *buf = logger->buf;
        initLogger(logger, NULL, 0);  // 清零
        longjmp(*buf, 1);
    }
    return re;
}

int writeFatalErrorLog_(Logger *logger, LogLoggerPrintConsole pc, char *file, int line, char *func, int exit_code, char *format, ...) {
    CHECK_LOGGER();

    va_list ap;
    va_start(ap, format);
    int re = writeLog_(logger, pc, log_fatal_error, file, line, func, format, ap);
    if (logger->process_fatal_error)
        exit(exit_code);
    return re;
}
