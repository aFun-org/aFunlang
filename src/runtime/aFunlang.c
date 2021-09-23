#include "__aFunlang.h"
#include "aFunCore.h"
#include "__env.h"

void aFunInit() {
    aFunCoreInit();
}

af_Environment *creatAFunEnviroment(void) {
    af_Environment *env = makeEnvironment(grt_count);
    /* 内置量操作 */
    return env;
}

void destructAFunEnvironment(af_Environment *env) {
    freeEnvironment(env);
}

static int runCode_(FilePath name, af_Parser *parser, int mode, af_Environment *env) {
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

int runCodeFromFile(FilePath file, FILE *error_file, af_Environment *env) {
    if (env == NULL || file == NULL)
        return -1;

    if (error_file == NULL)
        error_file = stderr;
    af_Parser *parser = makeParserByFile(file, error_file);
    return runCode_(file, parser, 1, env);
}

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
