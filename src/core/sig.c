#include "aFunCore.h"

static volatile sig_atomic_t sig_int = 0;  // SIGINT (Windows & *nix)
static volatile sig_atomic_t sig_term = 0;  // SIGTERM (*nix)

#if (defined SIGUSR1 && defined SIGUSR2)
static volatile sig_atomic_t sig_u1 = 0;  // SIGUSR1 (*nix)
static volatile sig_atomic_t sig_u2 = 0;  // SIGUSR1 (*nix)
#endif

static void aFunSigFunc(int signum);
static void aFunSigFunc(int signum) {
    switch (signum) {
        case SIGINT:
            sig_int = 1;
            break;
        case SIGTERM:
            sig_term = 1;
            break;
#if (defined SIGUSR1 && defined SIGUSR2)
        case SIGUSR1:
            sig_u1 = 1;
            break;
        case SIGUSR2:
            sig_u2 = 1;
            break;
#endif
        default:
            break;
    }
}

bool aFunGetSignal(int signum) {
    bool re = false;
    switch (signum) {
        case SIGINT:
            re = sig_int == 1;
            sig_int = 0;
            signal(SIGINT, aFunSigFunc);
            break;
        case SIGTERM:
            re = sig_term == 1;
            sig_term = 0;
            signal(SIGTERM, aFunSigFunc);
            break;
#if (defined SIGUSR1 && defined SIGUSR2)
        case SIGUSR1:
            re = sig_u1 == 1;
            sig_u1 = 0;
            signal(SIGUSR1, aFunSigFunc);
            break;
        case SIGUSR2:
            re = sig_u2 == 1;
            sig_u2 = 0;
            signal(SIGUSR2, aFunSigFunc);
            break;
#endif
        default:
            break;
    }

    return re;
}

void aFunSignalInit(af_SignalInfo *sig_info) {
    writeDebugLog(aFunCoreLogger, "Signal setting");

    sig_int = 0;
    sig_info->sig_int = signal(SIGINT, aFunSigFunc);
    assertWarningLog(sig_info->sig_int != SIG_ERR, aFunCoreLogger, "SIGINT setting fail");

    sig_term = 0;
    sig_info->sig_term = signal(SIGTERM, aFunSigFunc);
    assertWarningLog(sig_info->sig_term != SIG_ERR, aFunCoreLogger, "SIGTERM setting fail");

#if (defined SIGUSR1 && defined SIGUSR2)
    sig_u1 = 0;
    sig_info->sig_u1 = signal(SIGUSR1, aFunSigFunc);
    assertWarningLog(sig_info->sig_u1 != SIG_ERR, aFunCoreLogger, "SIGUSR1 setting fail");

    sig_u2 = 0;
    sig_info->sig_u2 = signal(SIGUSR2, aFunSigFunc);
    assertWarningLog(sig_info->sig_u2 != SIG_ERR, aFunCoreLogger, "SIGUSR2 setting fail");
#endif
}

void aFunSignalRecover(af_SignalInfo *sig_info) {
    writeDebugLog(aFunCoreLogger, "Signal recover");

    void *re = NULL;
    if (sig_info->sig_int != SIG_ERR) {
        re = signal(SIGINT, sig_info->sig_int);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGINT recover fail");
    }

    if (sig_info->sig_term != SIG_ERR) {
        re = signal(SIGTERM, sig_info->sig_term);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGTERM recover fail");
    }

#if (defined SIGUSR1 && defined SIGUSR2)
    if (sig_info->sig_u1 != SIG_ERR) {
        re = signal(SIGUSR1, sig_info->sig_u1);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGUSR1 recover fail");
    }

    if (sig_info->sig_u2 != SIG_ERR) {
        re = signal(SIGUSR2, sig_info->sig_u2);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGUSR2 recover fail");
    }
#endif
}
