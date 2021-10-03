#ifndef AFUN_INIT_H
#define AFUN_INIT_H
#include "aFunCoreExport.h"
#include "stdbool.h"
#include <setjmp.h>
#include "tool.h"

typedef struct aFunCoreInitInfo aFunCoreInitInfo;
struct aFunCoreInitInfo {
    char *base_dir;

    bool fe;
    bool se;
    jmp_buf *buf;
    LogLevel level;
};

AFUN_CORE_EXPORT extern Logger *aFunCoreLogger;
AFUN_CORE_EXPORT bool aFunCoreInit(aFunCoreInitInfo *info);

#endif //AFUN_INIT_H
