#include "aFunCore.hpp"
#include "__aFunlang.hpp"
#include "__env.hpp"
#include "__cycle_obj.hpp"

#ifdef aFunWIN32_NO_CYGWIN
#include <io.h>
#define fileno _fileno
#define isatty _isatty
#else
#include "unistd.h"
#endif

static int runCode_(af_Parser *parser, int mode, FilePath save_path, af_Environment *env);
static bool aFunInit_mark = false;

typedef struct guardian_GC_data guardian_GC_data;
struct guardian_GC_data{
    time_t last_time;
};

/* 内置守护器 */
static bool checkSignal(int signum,
                        const char *sig, const char *sigcfg, const char *sigerr, char err[],
                        af_Environment *env);
static af_GuardianList *guardian_GC(char *type, bool is_guard, guardian_GC_data *data, af_Environment *env);
static af_GuardianList *guardian_Signal(char *type, bool is_guard, void *data, af_Environment *env);

bool aFunInit(aFunInitInfo *info) {
    if (aFunInit_mark)
        return false;

    if (info == nullptr) {
        static aFunInitInfo info_default = {
                .base_dir=".",
                .log_asyn=true,
                .buf=nullptr,
                .level=log_info
        };
        info = &info_default;
    }

    aFunCoreInitInfo core_info = {.base_dir=info->base_dir,
                                  .log_asyn=info->log_asyn,
                                  .buf=info->buf,
                                  .level=info->level};

    aFunInit_mark = aFunCoreInit(&core_info);
    if (aFunInit_mark)
        writeDebugLog(aFunCoreLogger, "aFun-runtime Init success");
    return aFunInit_mark;
}

bool aFunDestruct() {
    return aFunCoreDestruct();
}

/*
 * 函数名: defineRunEnv
 * 目标: 运行前的初始化
 */
void defineRunEnv(aFunRunInfo *run_env) {
    aFunRunCoreInfo core_info = {.signal=run_env->signal};
    defineRunEnvCore(&core_info);
}

/*
 * 函数名: undefRunEnv
 * 目标: 取消运行前的初始化
 */
void undefRunEnv(aFunRunInfo *run_env) {
    aFunRunCoreInfo core_info = {.signal=run_env->signal};
    undefRunEnvCore(&core_info);
}


static bool checkSignal(int signum,
                        const char *sig, const char *sigcfg, const char *sigerr, char err[],
                        af_Environment *env) {
    bool re = aFunGetSignal(signum);
    if (!re)
        return false;
    int32_t *p_cfg = findEnvVarNumber(sigcfg, env);
    int32_t cfg = 0;
    if (p_cfg != nullptr)
        cfg = *p_cfg;

    if (cfg == 0) {  // 诱发错误
        strncat(err, sigerr, 218);
        setEnvVarNumber(sig, 0, env);
    } else if (cfg == 1) {  // 设置环境变量
        setEnvVarNumber(sig, 1, env);
    } else  // 忽略
        setEnvVarNumber(sig, 0, env);

    writeDebugLog(aFunCoreLogger, "Get %s as cfg %d", sig, cfg);
    return true;
}

/*
 * 函数名: checkRunGC
 * 目标: 检查是否该运行gc, 若是则返回true并运行gc, 否则返回false
 */
static af_GuardianList *guardian_GC(char *type, bool is_guard, guardian_GC_data *data, af_Environment *env) {
    time_t now = clock();
    if ((now - data->last_time) < 1 * CLOCKS_PER_SEC)  // 间隔 1s 再执行
        return nullptr;
    data->last_time = now;

    af_GuardianList *gl = gc_RunGC(env);
    if (gl != nullptr)
        writeDebugLog(aFunCoreLogger, "GC run destruct function");
    return gl;
}

static af_GuardianList *guardian_Signal(char *type, bool is_guard, void *data, af_Environment *env) {
    char error_msg[218] = {NUL};
    checkSignal(SIGINT, ev_sigint, ev_sigint_cfg, SIGNAL_INT, error_msg, env);
    checkSignal(SIGTERM, ev_sigterm, ev_sigterm_cfg, SIGNAL_TERM, error_msg, env);
#if (defined SIGUSR1 && defined SIGUSR2)
    checkSignal(SIGUSR1, ev_sigu1, ev_sigu1_cfg, SIGNAL_U1, error_msg, env);
    checkSignal(SIGUSR2, ev_sigu2, ev_sigu2_cfg, SIGNAL_U2, error_msg, env);
#endif

    if (*error_msg != NUL) {
        // error_msg 有内容写入, 需要处理
        if (env->activity->msg_down != nullptr) {
            af_Message *msg;
            if (EQ_STR("NORMAL", env->activity->msg_down->type)) {
                msg = getFirstMessage(env);
                gc_delObjectReference(*(af_Object **)msg->msg, env);
                freeMessage(msg);
            } else if (EQ_STR("ERROR", env->activity->msg_down->type)) {
                msg = getFirstMessage(env);
                freeErrorInfo(*(af_ErrorInfo **) msg->msg, env);
                freeMessage(msg);
            }
        }

        pushMessageDown(makeERRORMessage(SIGNAL_EXCEPTION, error_msg, env), env);
    }
    return nullptr;
}


af_Environment *creatAFunEnvironment(int argc, char **argv){
    if (!aFunInit_mark)
        return nullptr;

    af_Environment *env = makeEnvironment(grt_always);
    af_Code *code = nullptr;

    for(int i = 0; i < argc; i++)
        writeTrackLog(aFunCoreLogger, "[aFunlang] Env-arg %d. %s", i, argv[i]);

    setArgc(argc, env);
    for (int i = 0; i < argc; i++) {
        char tmp[512] = {0};
        snprintf(tmp, 512, ev_argvx_prefix "%d", i);
        setEnvVarData(tmp, argv[i], env);
    }

    runtimeTool("base", &code, nullptr, env->protect, env);

    if (code != nullptr) {
        bool res = iterCode(code, 0, env);
        freeAllCode(code);
        if (!res) {
            freeEnvironment(env);
            return nullptr;
        }
    }

    af_Object *cycle = makeCycleObject(env);  // gc 使用的函数
    makeVarToProtectVarSpace(mg_sys_cycle, 3, 3, 3, cycle, env);
    gc_delObjectReference(cycle, env);

    enableEnvironment(env);
    if (!env->is_derive) {  // 派生 gc 线程
        setVarSpaceProtect(nullptr, env->protect, true);

        af_Environment *gc_env = startRunThread(env, nullptr, nullptr, true, true, false, true, false);

        /* 设置守护器 */
        DLC_SYMBOL(GuardianFunc) func1 = MAKE_SYMBOL(guardian_Signal, GuardianFunc);
        addGuardian("SIGNAL", false, false, 0, func1, nullptr, nullptr, gc_env);
        FREE_SYMBOL(func1);

        DLC_SYMBOL(GuardianFunc) func2 = MAKE_SYMBOL(guardian_GC, GuardianFunc);
        addGuardian("GC", true, false, sizeof(guardian_GC_data),
                    func2, nullptr, nullptr, gc_env);
        FREE_SYMBOL(func2);

        gc_env->status = core_normal;  // 手动启动env
        af_Code *bt1 = makeElementCode(mg_sys_cycle, NUL, 1, "gc.aun.sys");
        af_Code *bt2 = makeBlockCode(curly, bt1, NUL, 1, "gc.aun.sys", nullptr);
        startRunThread_(gc_env, bt2, true);
        env->gc_env = gc_env;
    }

    return env;
}

bool destructAFunEnvironment(af_Environment *env) {
    waitForEnviromentExit(env);
    return freeEnvironment(env);
}

static int runCode_(af_Parser *parser, int mode, FilePath save_path, af_Environment *env){
    if (parser == nullptr)
        return -1;

    af_Code *bt_code = parserCode(parser);
    freeParser(parser);
    if (bt_code == nullptr)
        return -2;

    /* 写入文件 */
    if (save_path != nullptr) {
        int res = writeByteCode(bt_code, save_path);
        if (res != 1) {
            writeErrorLog(aFunCoreLogger, "Save %s bytecode error: %s", save_path, writeByteCodeError[res]);
            printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_save_e, "Save aFun Bytecode file error"), save_path);
        }
    }

    bool res = iterCode(bt_code, mode, env);
    freeAllCode(bt_code);
    if (!res)
        return getCoreExitCode(env);
    return 0;
}


/*
 * 函数名: runCodeFromString
 * 目标: 运行字符串中的程序 (源码形式)
 */
int runCodeFromString(const char *code, const char *string_name, int mode, af_Environment *env){
    if (env == nullptr || code == nullptr || !aFunInit_mark)
        return -1;

    if (string_name == nullptr)
        string_name = "string-code.aun";

    af_Parser *parser = makeParserByString(string_name, code, false);
    return runCode_(parser, mode, nullptr, env);
}

/*
 * 函数名: runCodeFromFileSource
 * 目标: 运行文件中的程序 (源码形式)
 */
int runCodeFromFileSource(FilePath file, bool save_afb, FilePath save_path, int mode, af_Environment *env){
    if (env == nullptr || file == nullptr || !aFunInit_mark)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == nullptr || !EQ_STR(".aun", sufix)) {
        writeErrorLog(aFunCoreLogger, "Source is not .aun file: %s", (sufix == nullptr ? "" : sufix));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_source_not_aub_e, "Source is not .aun file"), (sufix == nullptr ? "" : sufix));
        return -2;
    }

    /* 若文件不存在则自动生成 */
    bool free_save_path = false;
    if (save_afb && !save_path) {
        char *path = getFileNameWithPath(file);
        save_path = strJoin(path, ".aub", true, false);
        free_save_path = true;
    } else if (!save_afb)
        save_path = nullptr;

    af_Parser *parser = makeParserByFile(file);
    int exit_code = runCode_(parser, mode, save_path, env);
    if (free_save_path)
        free(save_path);
    return exit_code;
}

/*
 * 函数名: runCodeFromStdin
 * 目标: 运行stdin的程序 (源码形式)
 */
int runCodeFromStdin(const char *name, af_Environment *env){
    if (env == nullptr || CLEAR_STDIN() || !aFunInit_mark || !isatty(fileno(stdin)))  // ferror在feof前执行
        return -1;

    if (name == nullptr)
        name = "sys-stdin.aun";

    af_Parser *parser = makeParserByStdin(name);
    return runCode_(parser, 0, nullptr, env);
}

/*
 * 函数名: runCodeFromMemory
 * 目标: 运行内存中的程序 (字节码形式)
 */
int runCodeFromMemory(af_Code *code, int mode, af_Environment *env){
    if (!aFunInit_mark)
        return -1;

    bool res = iterCode(code, mode, env);
    if (!res)
        return getCoreExitCode(env);
    return 0;
}

/*
 * 函数名: runCodeFromFileByte
 * 目标: 运行文件中的程序 (字节码形式)
 */
int runCodeFromFileByte(FilePath file, int mode, af_Environment *env){
    if (env == nullptr || file == nullptr || !aFunInit_mark)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == nullptr || !EQ_STR(".aub", sufix)) {
        writeErrorLog(aFunCoreLogger, "Bytecode not .aub file: %s", (sufix == nullptr ? "" : sufix));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_bt_not_aub_e, "Bytecode not .aub file"), (sufix == nullptr ? "" : sufix));
        return -2;
    }

    af_Code *code = nullptr;
    int res = readByteCode(&code, file);
    if(res != 1) {
        writeErrorLog(aFunCoreLogger, "Load %s bytecode file error: %s", file, readByteCodeError[res]);
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_load_bt_e, "Load bytecode file error"), file);
        return -2;
    }

    int exit_code = runCodeFromMemory(code, mode, env);
    freeAllCode(code);
    return exit_code;
}

/*
 * 函数名: runCodeFromFileByte
 * 目标: 运行文件中的程序 (字节码/源码形式)
 */
int runCodeFromFile(FilePath file, bool save_afb, int mode, af_Environment *env){
    if (env == nullptr || file == nullptr || !aFunInit_mark)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix != nullptr && !EQ_STR(".aun", sufix) && !EQ_STR(".aub", sufix)) {  // 不是源文件, 字节码文件或无后缀文件
        writeErrorLog(aFunCoreLogger, "Run file not .aun/.aub file: %s", sufix);
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_file_aun_aub_e, "Run file not .aun/.aub file"), file);
        return -2;
    }

    char *path = getFileNameWithPath(file);
    char *path_1 = strJoin(path, ".aun", false, false);
    char *path_2 = strJoin(path, ".aub", true, false);  // 此时释放path

    time_t time_1 = getFileMTime(path_1);
    time_t time_2 = getFileMTime(path_2);

    if (time_1 == 0 && time_2 == 0) {
        writeErrorLog(aFunCoreLogger, "Run file not exists: %s", file);
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_file_not_exists_e, "Run file not exists"), file);
        free(path_1);
        free(path_2);
        return -3;
    }

    int exit_code;
    if (time_2 >= time_1) {
        exit_code = runCodeFromFileByte(path_2, mode, env);
        if (exit_code != 0)
            goto RUN_SOURCE_CODE;
    } else {
RUN_SOURCE_CODE:
        exit_code = runCodeFromFileSource(path_1, save_afb, path_2, mode, env);
    }

    free(path_1);
    free(path_2);
    return exit_code;
}

/*
 * 函数名: buildFile
 * 目标: 生成字节码文件
 */
int buildFile(FilePath out, FilePath in){
    if (out == nullptr || in == nullptr || !aFunInit_mark)
        return -1;

    char *suffix_in = getFileSurfix(in);
    char *suffix_out = getFileSurfix(out);
    if (suffix_in == nullptr || !EQ_STR(".aun", suffix_in)) {  // 不是源文件
        writeErrorLog(aFunCoreLogger, "Input not .aun %s", (suffix_in == nullptr ? "" : suffix_in));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(build_in_aun_e, "Input file is not .aun file"), (suffix_in == nullptr ? "" : suffix_in));
        return -2;
    }

    if (suffix_out == nullptr || !EQ_STR(".aub", suffix_out)) {  // 不是字节码文件
        writeErrorLog(aFunCoreLogger, "Output not .aub %s", (suffix_out == nullptr ? "" : suffix_out));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(build_out_aub_e, "Output file is not .aub file"), (suffix_out == nullptr ? "" : suffix_out));
        return -2;
    }

    af_Parser *parser = makeParserByFile(in);
    af_Code *code = parserCode(parser);
    freeParser(parser);
    if (code == nullptr)
        return -2;

    int res = writeByteCode(code, out);
    freeAllCode(code);

    if (res != 1) {
        writeErrorLog(aFunCoreLogger, "Build %s error: %s", in, writeByteCodeError[res]);
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(build_error_e, "Build error"), in);
        return -3;
    }

    return 0;
}
