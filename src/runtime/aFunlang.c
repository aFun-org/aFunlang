#include "__aFunlang.h"
#include "aFunCore.h"
#include "__env.h"

static int runCode_(FilePath name, af_Parser *parser, int mode, af_Environment *env);

void aFunInit() {
    aFunCoreInit();
}

af_Environment *creatAFunEnviroment(void) {
    af_Environment *env = makeEnvironment(grt_count);
    af_Code *code;

    aFunTool("base", &code, NULL, env->core->protect, env);

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

static int runCode_(FilePath name, af_Parser *parser, int mode, af_Environment *env) {
    if (parser == NULL)
        return -1;

    af_Code *bt_code = parserCode(name, parser);
    freeParser(parser);
    if (bt_code == NULL)
        return -2;

    bool res = iterCode(bt_code, mode, env);
    freeAllCode(bt_code);
    if (!res)
        return env->core->exit_code;
    return 0;
}


/*
 * 函数名: runCodeFromString
 * 目标: 运行字符串中的程序 (源码形式)
 */
int runCodeFromString(char *code, char *string_name, FILE *error_file, af_Environment *env) {
    if (env == NULL || code == NULL)
        return -1;

    if (string_name == NULL)
        string_name = "string-code.af";

    if (error_file == NULL)
        error_file = stderr;
    af_Parser *parser = makeParserByString(code, false, error_file);
    return runCode_(string_name, parser, 1, env);
}

/*
 * 函数名: runCodeFromFileSource
 * 目标: 运行文件中的程序 (源码形式)
 */
int runCodeFromFileSource(FilePath file, FILE *error_file, af_Environment *env) {
    if (env == NULL || file == NULL)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == NULL || !EQ_STR(".af", sufix))
        return -2;

    if (error_file == NULL)
        error_file = stderr;
    af_Parser *parser = makeParserByFile(file, error_file);
    return runCode_(file, parser, 1, env);
}

/*
 * 函数名: runCodeFromStdin
 * 目标: 运行stdin的程序 (源码形式)
 */
int runCodeFromStdin(char *name, FILE *error_file, af_Environment *env) {
    if (env == NULL || feof(stdin) || ferror(stdin))
        return -1;

    if (name == NULL)
        name = "sys-stdin.af";

    if (error_file == NULL)
        error_file = stderr;
    af_Parser *parser = makeParserByStdin(error_file);
    return runCode_(name, parser, 0, env);
}

/*
 * 函数名: runCodeFromMemory
 * 目标: 运行内存中的程序 (字节码形式)
 */
int runCodeFromMemory(af_Code *code, af_Environment *env) {
    bool res = iterCode(code, 0, env);
    if (!res)
        return env->core->exit_code;
    return 0;
}

/*
 * 函数名: runCodeFromMemoryAsImport
 * 目标: 采用import的方式运行内存中程序 (字节码形式)
 */
int runCodeFromMemoryAsImport(af_Code *code, af_Environment *env) {
    bool res = iterCode(code, 1, env);
    if (!res)
        return env->core->exit_code;
    return 0;
}

/*
 * 函数名: runCodeFromFileByte
 * 目标: 运行文件中的程序 (字节码形式)
 */
int runCodeFromFileByte(FilePath file, FILE *error_file, af_Environment *env) {
    if (env == NULL || file == NULL)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == NULL || !EQ_STR(".afb", sufix))
        return -2;

    if (error_file == NULL)
        error_file = stderr;
    af_Code *code;
    FILE *file_ = fopen(file, "rb");
    if (file_ == NULL) {
        return -3;
    }

    if(!readAllCode(&code, file_)) {
        freeAllCode(code);
        return -2;
    }

    int exit_code = runCodeFromMemoryAsImport(code, env);
    freeAllCode(code);

    return exit_code;
}

/*
 * 函数名: runCodeFromFileByte
 * 目标: 运行文件中的程序 (字节码/源码形式)
 * 注意: 必须传入.af文件
 */
int runCodeFromFile(FilePath file, FILE *error_file, af_Environment *env) {
    if (env == NULL || file == NULL)
        return -1;

    char *sufix = getFileSurfix(file);
    if (sufix == NULL || !EQ_STR(".af", sufix))
        return -2;

    char *path = getFileNameWithPath(file);
    char *path_1 = strJoin(path, ".af", false, false);
    char *path_2 = strJoin(path, ".afb", true, false);  // 此时释放path

    time_t time_1 = getFileMTime(path_1);
    time_t time_2 = getFileMTime(path_2);

    if (time_1 == 0 && time_2 == 0)
        return -3;

    int exit_code;
    if (time_2 >= time_1)
        exit_code = runCodeFromFileByte(path_2, error_file, env);
    else
        exit_code = runCodeFromFileSource(path_1, error_file, env);

    free(path_1);
    free(path_2);
    return exit_code;
}
