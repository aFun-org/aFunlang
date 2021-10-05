﻿#ifndef AFUN_SIG_H
#define AFUN_SIG_H
#include <signal.h>
#include "aFunCoreExport.h"

struct af_SignalInfo {
    void *sig_int;
    void *sig_term;
#if (defined SIGUSR1 && defined SIGUSR2)
    void *sig_u1;
    void *sig_u2;
#endif
};

typedef struct af_SignalInfo af_SignalInfo;

AFUN_CORE_EXPORT void aFunSignalInit(af_SignalInfo *sig_info);
AFUN_CORE_EXPORT void aFunSignalRecover(af_SignalInfo *sig_info);
AFUN_CORE_EXPORT bool aFunGetSignal(int signum);

#endif //AFUN_SIG_H
