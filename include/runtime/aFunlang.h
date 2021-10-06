#ifndef AFUN_AFUNLANG_H
#define AFUN_AFUNLANG_H
#include "aFunlangExport.h"
#include "aFunCore.h"
#include "runtime.h"

typedef struct aFunInitInfo aFunInitInfo;
struct aFunInitInfo {
    char *base_dir;

    jmp_buf *buf;
    LogLevel level;
};

AFUN_LANG_EXPORT bool aFunInit(aFunInitInfo *info);

AFUN_LANG_EXPORT af_Environment *creatAFunEnvironment(int argc, char **argv);
AFUN_LANG_EXPORT void destructAFunEnvironment(af_Environment *env);

/* 源文件运行 */
AFUN_LANG_EXPORT int runCodeFromString(char *code, char *string_name, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFileSource(FilePath file, bool save_afb, FilePath save_path, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromStdin(char *name, ParserStdinInterruptFunc *interrupt, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromMemory(af_Code *code, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFileByte(FilePath file, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFile(FilePath file, bool save_afb, int mode, af_Environment *env);
AFUN_LANG_EXPORT int buildFile(FilePath out, FilePath in);
#endif //AFUN_AFUNLANG_H
