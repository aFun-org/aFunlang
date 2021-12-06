/*
 * 文件名: core_init.c
 * 目标: 初始化函数
 */

#include <clocale>
#include "core_init.hpp"
#include "__sig.hpp"
#include "tool.hpp"

static Logger aFunCoreLogger_;
Logger *aFunCoreLogger = &aFunCoreLogger_;

char *log_path = nullptr;
char *lang_path = nullptr;
char *varlib_path = nullptr;

static void destructCoreExit(void *data) {
    free(log_path);
    free(lang_path);
    free(varlib_path);
}

bool aFunCoreInit(aFunCoreInitInfo *info) {
    if (info == nullptr) {
        static aFunCoreInitInfo info_default = {.base_dir=".",
                                                .log_asyn=true,
                                                .buf=nullptr,
                                                .level=log_info};
        info = &info_default;
    }

    getEndian();
    if (setlocale(LC_ALL, "") == nullptr)
        return false;
    if (info->base_dir == nullptr)
        return false;

    log_path = strJoin(info->base_dir, SEP aFunLogDir SEP, false, false);
    lang_path = strJoin(info->base_dir, SEP aFunLangDir SEP, false, false);
    varlib_path = strJoin(info->base_dir, SEP aFunVarLibDir SEP, false, false);
    aFunAtExit(destructCoreExit, nullptr);

    char *log = strJoin(log_path, "aFunlang", false, false);
    bool re = initLogSystem(log, info->log_asyn);
    free(log);
    if (re == 0)
        return false;

    initLogger(aFunCoreLogger, "aFunlang-core", info->level);
    aFunCoreLogger->buf = info->buf;

    writeDebugLog(aFunCoreLogger, "aFunCore log path: %s", log_path);
    writeDebugLog(aFunCoreLogger, "aFunCore var.lib path: %s", varlib_path);
    writeDebugLog(aFunCoreLogger, "aFunCore lang path: %s", lang_path);

    char LANG_path[218] = {0};
    snprintf(LANG_path, 218, "%sLANG", lang_path);

    FILE *LANG_file = fileOpen(LANG_path, "r");
    if (LANG_file != nullptr) {
        char LANG[100] = {0};
        fgets(LANG, 100, LANG_file);
        if (LANG[strlen(LANG) - 1] == '\n')
            LANG[strlen(LANG) - 1] = NUL;  // 去除`\n`
        writeDebugLog(aFunCoreLogger, "language = %s", LANG);

        char LANG_lib[218] = {0};
        snprintf(LANG_lib, 218, "%s" SHARED_PREFIX "%s" SHARED_SUFFIX, lang_path, LANG);
        if (HT_initaFunGetText(LANG_lib) == 0)
            writeDebugLog(aFunCoreLogger, "aFunCore lang init success: %s", LANG_lib);
        else
            writeDebugLog(aFunCoreLogger, "aFunCore lang init failed: %s", LANG_lib);
        fileClose(LANG_file);
    } else
        HT_initaFunGetText(nullptr);
    writeDebugLog(aFunCoreLogger, "aFunCore init success");
    return true;
}

bool aFunCoreDestruct() {
    destructLogSystem();
    return true;
}

void defineRunEnvCore(aFunRunCoreInfo *run_env) {
    if (run_env->signal)
        aFunSignalInit();
}

void undefRunEnvCore(aFunRunCoreInfo *run_env) {
    if (run_env->signal)
        aFunSignalRecover();
}
