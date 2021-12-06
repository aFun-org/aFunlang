#ifndef AFUN_INIT_H
#define AFUN_INIT_H
#include "aFunCoreExport.h"
#include <setjmp.h>
#include "macro.hpp"
#include "sig.hpp"
#include "tool.hpp"

typedef struct aFunCoreInitInfo aFunCoreInitInfo;
struct aFunCoreInitInfo {
    const char *base_dir;

    bool log_asyn;
    jmp_buf *buf;
    LogLevel level;
};

typedef struct aFunRunCoreInfo aFunRunCoreInfo;
struct aFunRunCoreInfo {
    bool signal;
};

AFUN_CORE_EXPORT extern Logger *aFunCoreLogger;
AFUN_CORE_EXPORT bool aFunCoreInit(aFunCoreInitInfo *info);
AFUN_CORE_EXPORT bool aFunCoreDestruct(void);

AFUN_CORE_EXPORT void defineRunEnvCore(aFunRunCoreInfo *run_env);
AFUN_CORE_EXPORT void undefRunEnvCore(aFunRunCoreInfo *run_env);

#endif //AFUN_INIT_H
