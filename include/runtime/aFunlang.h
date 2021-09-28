#ifndef AFUN_AFUNLANG_H
#define AFUN_AFUNLANG_H
#include "aFunlangExport.h"
#include "aFunCore.h"

#include "runtime.h"

AFUN_LANG_EXPORT void aFunInit();

AFUN_LANG_EXPORT af_Environment *creatAFunEnviroment(void);
AFUN_LANG_EXPORT void destructAFunEnvironment(af_Environment *env);

/* 源文件运行 */
AFUN_LANG_EXPORT int runCodeFromString(char *code, char *string_name, FILE *error_file, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFileSource(FilePath file, FILE *error_file, bool save_afb, FilePath save_path, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromStdin(char *name, FILE *error_file, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromMemory(af_Code *code, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromMemoryAsImport(af_Code *code, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFileByte(FilePath file, FILE *error_file, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFile(FilePath file, FILE *error_file, bool save_afb, af_Environment *env);
AFUN_LANG_EXPORT int buildFile(FilePath out, FilePath in, FILE *error_file);
#endif //AFUN_AFUNLANG_H
