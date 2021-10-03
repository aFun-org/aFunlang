#include "aFunCore.h"
#include "__aFunlang.h"
#include "__env.h"

static int runCode_(FilePath name, af_Parser *parser, int mode, FilePath save_path, af_Environment *env);
static bool aFunInit_mark = false;

bool aFunInit(aFunInitInfo *info) {
    if (aFunInit_mark)
        return false;

    if (info == NULL) {
        static aFunInitInfo info_default = {
                .base_dir=".",
                .buf=NULL,
                .level=log_info
        };
        info = &info_default;
    }

    aFunCoreInitInfo core_info = {.base_dir=info->base_dir,
                                  .buf=info->buf,
                                  .level=info->level};

    aFunInit_mark = aFunCoreInit(&core_info);
    if (aFunInit_mark)
        writeDebugLog(aFunCoreLogger, "aFun-runtime Init success");
    return aFunInit_mark;
}

af_Environment *creatAFunEnvironment(int argc, char **argv){
    if (!aFunInit_mark)
        return NULL;

    af_Environment *env = makeEnvironment(grt_count);
    af_Code *code = NULL;

    for(int i = 0; i < argc; i++)
        writeTrackLog(aFunCoreLogger, "[aFunlang] Env-arg %d. %s", i, argv[i]);

    env->core->argc->num = argc;
    for (int i = 0; i < argc; i++) {
        char tmp[512] = {0};
        snprintf(tmp, 512, ev_argvx_prefix "%d", i);
        setEnvVarData(tmp, argv[i], env);
    }

    runtimeTool("base", &code, NULL, env->core->protect, env);

    if (code != NULL) {
        bool res = iterCode(code, 0, env);
        freeAllCode(code);
        if (!res) {
            freeEnvironment(env);
            return NULL;
        }
    }

    enableEnvironment(env);
    return env;
}

void destructAFunEnvironment(af_Environment *env) {
    freeEnvironment(env);
}

static int runCode_(FilePath name, af_Parser *parser, int mode, FilePath save_path, af_Environment *env){
    if (parser == NULL)
        return -1;

    af_Code *bt_code = parserCode(name, parser);
    freeParser(parser);
    if (bt_code == NULL)
        return -2;

    /* 写入文件 */
    if (save_path != NULL) {
        int res = writeByteCode(bt_code, save_path);
        if (res != 1) {
            writeErrorLog(aFunCoreLogger, "Save %s bytecode error: %s", save_path, writeByteCodeError[res]);
            printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_save_e, "Save aFun Bytecode file error"), save_path);
        }
    }

    bool res = iterCode(bt_code, mode, env);
    freeAllCode(bt_code);
    if (!res)
        return env->core->exit_code_->num;
    return 0;
}


/*
 * 函数名: runCodeFromString
 * 目标: 运行字符串中的程序 (源码形式)
 */
int runCodeFromString(char *code, char *string_name, int mode, af_Environment *env){
    if (env == NULL || code == NULL || !aFunInit_mark)
        return -1;

    if (string_name == NULL)
        string_name = "string-code.aun";

    af_Parser *parser = makeParserByString(code, false);
    return runCode_(string_name, parser, mode, NULL, env);
}

/*
 * 函数名: runCodeFromFileSource
 * 目标: 运行文件中的程序 (源码形式)
 */
int runCodeFromFileSource(FilePath file, bool save_afb, FilePath save_path, int mode, af_Environment *env){
    if (env == NULL || file == NULL || !aFunInit_mark)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == NULL || !EQ_STR(".aun", sufix)) {
        writeErrorLog(aFunCoreLogger, "Source is not .aun file: %s", (sufix == NULL ? "" : sufix));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_source_not_aub_e, "Source is not .aun file"), (sufix == NULL ? "" : sufix));
        return -2;
    }

    /* 若文件不存在则自动生成 */
    bool free_save_path = false;
    if (save_afb && !save_path) {
        char *path = getFileNameWithPath(file);
        save_path = strJoin(path, ".aub", true, false);
        free_save_path = true;
    } else if (!save_afb)
        save_path = NULL;

    af_Parser *parser = makeParserByFile(file);
    int exit_code = runCode_(file, parser, mode, save_path, env);
    if (free_save_path)
        free(save_path);
    return exit_code;
}

/*
 * 函数名: runCodeFromStdin
 * 目标: 运行stdin的程序 (源码形式)
 */
int runCodeFromStdin(char *name, af_Environment *env){
    if (env == NULL || feof(stdin) || ferror(stdin) || !aFunInit_mark)
        return -1;

    if (name == NULL)
        name = "sys-stdin.aun";

    af_Parser *parser = makeParserByStdin();
    return runCode_(name, parser, 0, NULL, env);
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
        return env->core->exit_code_->num;
    return 0;
}

/*
 * 函数名: runCodeFromFileByte
 * 目标: 运行文件中的程序 (字节码形式)
 */
int runCodeFromFileByte(FilePath file, int mode, af_Environment *env){
    if (env == NULL || file == NULL || !aFunInit_mark)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == NULL || !EQ_STR(".aub", sufix)) {
        writeErrorLog(aFunCoreLogger, "Bytecode not .aub file: %s", (sufix == NULL ? "" : sufix));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(run_bt_not_aub_e, "Bytecode not .aub file"), (sufix == NULL ? "" : sufix));
        return -2;
    }

    af_Code *code = NULL;
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
    if (env == NULL || file == NULL || !aFunInit_mark)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix != NULL && !EQ_STR(".aun", sufix) && !EQ_STR(".aub", sufix)) {  // 不是源文件, 字节码文件或无后缀文件
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
    if (out == NULL || in == NULL || !aFunInit_mark)
        return -1;

    char *suffix_in = getFileSurfix(in);
    char *suffix_out = getFileSurfix(out);
    if (suffix_in == NULL || !EQ_STR(".aun", suffix_in)) {  // 不是源文件
        writeErrorLog(aFunCoreLogger, "Input not .aun %s", (suffix_in == NULL ? "" : suffix_in));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(build_in_aun_e, "Input file is not .aun file"), (suffix_in == NULL ? "" : suffix_in));
        return -2;
    }

    if (suffix_out == NULL || !EQ_STR(".aub", suffix_out)) {  // 不是字节码文件
        writeErrorLog(aFunCoreLogger, "Output not .aub %s", (suffix_out == NULL ? "" : suffix_out));
        printf_stderr(0, "%s: %s\n", HT_aFunGetText(build_out_aub_e, "Output file is not .aub file"), (suffix_out == NULL ? "" : suffix_out));
        return -2;
    }

    af_Parser *parser = makeParserByFile(in);
    af_Code *code = parserCode(in, parser);
    freeParser(parser);
    if (code == NULL)
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
