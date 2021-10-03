/*
 * 文件名: core_init.c
 * 目标: 初始化函数
 */

#include "core_init.h"
#include "tool.h"
#include <locale.h>

static Logger aFunCoreLogger_;
Logger *aFunCoreLogger = &aFunCoreLogger_;

char *log_path = NULL;
char *lang_path = NULL;
char *varlib_path = NULL;

static void destructCoreExit(void) {
    free(log_path);
    free(lang_path);
    free(varlib_path);
}

bool aFunCoreInit(aFunCoreInitInfo *info) {
    if (info == NULL) {
        static aFunCoreInitInfo info_default = {.base_dir=".",
                                                .fe=true,
                                                .se=true,
                                                .buf=NULL,
                                                .level=log_info};
        info = &info_default;
    }

    getEndian();
    if (setlocale(LC_ALL, "") == NULL)
        return false;
    if (info->base_dir == NULL)
        return false;

    log_path = strJoin(info->base_dir, SEP aFunLogDir SEP, false, false);
    lang_path = strJoin(info->base_dir, SEP aFunLangDir SEP, false, false);
    varlib_path = strJoin(info->base_dir, SEP aFunVarLibDir SEP, false, false);
    atexit(destructCoreExit);

    char *log = strJoin(log_path, "aFunlang", false, false);
    bool re = initLogSystem(log);
    free(log);
    if (re == 0)
        return false;

    initLogger(aFunCoreLogger, "aFunlang-core", info->level);
    aFunCoreLogger->process_send_error = info->fe;
    aFunCoreLogger->process_fatal_error = info->se;
    aFunCoreLogger->buf = info->buf;

    writeDebugLog(aFunCoreLogger, "aFunCore init success");
    writeDebugLog(aFunCoreLogger, "aFunCore log path: %s", log_path);
    writeDebugLog(aFunCoreLogger, "aFunCore var/lib path: %s", varlib_path);
    writeDebugLog(aFunCoreLogger, "aFunCore lang path: %s", lang_path);

    char LANG[100] = {0};
    char *LANG_path = strJoin(lang_path, "LANG", false, false);
    FILE *LANG_file = fopen(LANG_path, "r");
    writeDebugLog(aFunCoreLogger, "LANG_path = %s", LANG_path);
    if (LANG_file != NULL) {
        fgets(LANG, 100, LANG_file);
        if (LANG[strlen(LANG) - 1] == '\n')
            LANG[strlen(LANG) - 1] = NUL;  // 去除`\n`
        writeDebugLog(aFunCoreLogger, "LANG = %s", LANG);

        char *LANG_lib = strJoin(lang_path, LANG, false, false);
        if (HT_initGetText(LANG_lib) == 0)
            writeDebugLog(aFunCoreLogger, "aFunCore lang init: %s", LANG_lib);
        free(LANG_lib);
        fclose(LANG_file);
    } else
        HT_initGetText(NULL);
    free(LANG_path);

    return true;
}
