/*
 * 文件名: core_init.c
 * 目标: 初始化函数
 */

#include "core_init.h"
#include "tool.h"
#include <locale.h>

static Logger aFunCoreLogger_;
Logger *aFunCoreLogger = &aFunCoreLogger_;

bool aFunCoreInit(char *log_dir, LogFactoryPrintConsole print_console, bool fe, bool se, jmp_buf *buf, LogLevel level) {
    getEndian();
    if (setlocale(LC_ALL, "") == NULL)
        return false;
#ifdef aFunWIN32
    if(!SetConsoleOutputCP(65001))  // 设置windows代码页为utf-8编码
        return false;
#endif
    if (log_dir == NULL)
        return false;
    char *log = strJoin(log_dir, "aFunlang-", false, false);
    bool re = initLogSystem(log, print_console);
    free(log);
    if (re == 0)
        return false;

    initLogger(aFunCoreLogger, "aFunlang-core", level);
    aFunCoreLogger->process_send_error = fe;
    aFunCoreLogger->process_fatal_error = se;
    aFunCoreLogger->buf = buf;

    writeInfoLog(aFunCoreLogger, "aFunCore init success");
    return true;
}
