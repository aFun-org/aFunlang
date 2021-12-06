#ifndef AFUN_SIG_HPP
#define AFUN_SIG_HPP
#include <signal.h>
#include "aFunCoreExport.h"

AFUN_CORE_EXPORT void aFunSignalInit(void);
AFUN_CORE_EXPORT void aFunSignalRecover(void);
AFUN_CORE_EXPORT bool aFunGetSignal(int signum);

#endif //AFUN_SIG_HPP
