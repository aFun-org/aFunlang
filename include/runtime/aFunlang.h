#ifndef AFUN_AFUNLANG_H
#define AFUN_AFUNLANG_H
#include "aFunlangExport.h"
#include "aFunCore.h"

AFUN_LANG_EXPORT void aFunInit();

AFUN_LANG_EXPORT af_Environment *creatAFunEnviroment(void);
AFUN_LANG_EXPORT void destructAFunEnvironment(af_Environment *env);

/* 源文件运行 */
AFUN_LANG_EXPORT int runCodeFromString(char *code, char *string_name, FILE *error_file, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFile(FilePath file, FILE *error_file, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromStdin(char *name, FILE *error_file, af_Environment *env);

#endif //AFUN_AFUNLANG_H
