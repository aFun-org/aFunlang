#ifndef AFUN_AFUNLANG_HPP
#define AFUN_AFUNLANG_HPP
#include "aFunlangExport.h"
#include "aFunCore.hpp"
#include "runtime.hpp"

typedef struct aFunInitInfo aFunInitInfo;
struct aFunInitInfo {
    const char *base_dir;

    bool log_asyn;
    jmp_buf *buf;
    LogLevel level;
};

typedef struct aFunRunInfo aFunRunInfo;
struct aFunRunInfo {
    bool signal;
};

AFUN_LANG_EXPORT bool aFunInit(aFunInitInfo *info);
AFUN_LANG_EXPORT bool aFunDestruct();

AFUN_LANG_EXPORT void defineRunEnv(aFunRunInfo *run_env);
AFUN_LANG_EXPORT void undefRunEnv(aFunRunInfo *run_env);

AFUN_LANG_EXPORT af_Environment *creatAFunEnvironment(int argc, char **argv);
AFUN_LANG_EXPORT bool destructAFunEnvironment(af_Environment *env);

/* 源文件运行 */
AFUN_LANG_EXPORT int runCodeFromString(const char *code, const char *string_name, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFileSource(FilePath file, bool save_afb, FilePath save_path, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromStdin(const char *name, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromMemory(af_Code *code, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFileByte(FilePath file, int mode, af_Environment *env);
AFUN_LANG_EXPORT int runCodeFromFile(FilePath file, bool save_afb, int mode, af_Environment *env);
AFUN_LANG_EXPORT int buildFile(FilePath out, FilePath in);
#endif //AFUN_AFUNLANG_HPP
