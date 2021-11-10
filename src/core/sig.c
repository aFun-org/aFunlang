#include "aFunCore.h"
#include "__sig.h"
#include "pthread.h"

struct af_SignalInfo {
    void *sig_int;
    void *sig_term;

    volatile sig_atomic_t flat_int;  // SIGUSR1 (*nix)
    volatile sig_atomic_t flat_term;  // SIGUSR1 (*nix)

#ifdef aFunWIN32_NO_CYGWIN

#else
    volatile sig_atomic_t flat_u1;  // SIGUSR1 (*nix)
    volatile sig_atomic_t flat_u2;  // SIGUSR1 (*nix)

    void *sig_u1;
    void *sig_u2;
    sigset_t new;
    sigset_t old;
#endif
};

static void aFuncheckSignal(void);
static void aFunSigFunc(int signum);
typedef struct af_SignalInfo af_SignalInfo;
af_SignalInfo sig_info;
bool init_sig_info = false;
pthread_mutex_t sig_mutex = PTHREAD_MUTEX_INITIALIZER;


void aFunSignalInit(void) {
    pthread_mutex_lock(&sig_mutex);
    if (init_sig_info) {
        pthread_mutex_unlock(&sig_mutex);
        writeWarningLog(aFunCoreLogger, "Double signal init");
        return;
    }

    writeDebugLog(aFunCoreLogger, "Signal init");
    init_sig_info = true;
#ifdef aFunWIN32_NO_CYGWIN
    sig_info->sig_int = signal(SIGINT, aFunSigFunc);
    if (aFunSigFunc != sig_info->sig_int)
        sig_int = 0;
    assertWarningLog(sig_info->sig_int != SIG_ERR, aFunCoreLogger, "SIGINT setting fail");

    sig_info->sig_term = signal(SIGTERM, aFunSigFunc);
    if (aFunSigFunc != sig_info->sig_term)
        sig_term = 0;
    assertWarningLog(sig_info->sig_term != SIG_ERR, aFunCoreLogger, "SIGTERM setting fail");
#else
    sigemptyset(&sig_info.old);
    sigemptyset(&sig_info.new);
    sigaddset(&sig_info.new, SIGINT);
    sigaddset(&sig_info.new, SIGTERM);
    sigaddset(&sig_info.new, SIGUSR1);
    sigaddset(&sig_info.new, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sig_info.new, &sig_info.old);

    sig_info.sig_int = signal(SIGINT, aFunSigFunc);
    sig_info.sig_term = signal(SIGTERM, aFunSigFunc);
    sig_info.sig_u1 = signal(SIGUSR1, aFunSigFunc);
    sig_info.sig_u2 = signal(SIGUSR2, aFunSigFunc);

    sig_info.flat_int = 0;
    sig_info.flat_term = 0;
    sig_info.flat_u1 = 0;
    sig_info.flat_u2 = 0;

    pthread_mutex_unlock(&sig_mutex);
#endif
}

void aFunSignalRecover(void) {
    pthread_mutex_lock(&sig_mutex);
    if (!init_sig_info) {
        pthread_mutex_unlock(&sig_mutex);
        writeWarningLog(aFunCoreLogger, "Signal recover before init");
        return;
    }

    void *re = NULL;
    writeDebugLog(aFunCoreLogger, "Signal recover");

    if (sig_info.sig_int != SIG_ERR) {
        re = signal(SIGINT, sig_info.sig_int);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGINT recover fail");
    }

    if (sig_info.sig_term != SIG_ERR) {
        re = signal(SIGTERM, sig_info.sig_term);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGTERM recover fail");
    }

#ifdef aFunWIN32_NO_CYGWIN

#else
    if (sig_info.sig_u1 != SIG_ERR) {
        re = signal(SIGUSR1, sig_info.sig_u1);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGUSR1 recover fail");
    }

    if (sig_info.sig_u2 != SIG_ERR) {
        re = signal(SIGUSR2, sig_info.sig_u2);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGUSR2 recover fail");
    }
    sigprocmask(SIG_SETMASK, &sig_info.old, NULL);
    pthread_mutex_unlock(&sig_mutex);
#endif
}

static void aFunSigFunc(int signum) {
    switch (signum) {
        case SIGINT:
            sig_info.flat_int++;
            break;
        case SIGTERM:
            sig_info.flat_term++;
            break;
#ifndef aFunWIN32_NO_CYGWIN
        case SIGUSR1:
            sig_info.flat_u1++;
            break;
        case SIGUSR2:
            sig_info.flat_u2++;
            break;
#endif
        default:
            break;
    }
}

static void aFuncheckSignal(void) {
    /* 该函数是内部函数, 不负责加锁 */
#ifdef aFunWIN32_NO_CYGWIN

#else
    for (int i = 0; i < 4; i++) {
        // 标准: sigprocmask 执行返回前必有一个未递达(如果有)的信号递达
        // 循环 count 次, 确保信号都递达
        signal(SIGINT, aFunSigFunc);
        signal(SIGTERM, aFunSigFunc);
        signal(SIGUSR1, aFunSigFunc);
        signal(SIGUSR2, aFunSigFunc);
        sigprocmask(SIG_SETMASK, &sig_info.old, NULL);  // 让信号递达
    }
    sigprocmask(SIG_BLOCK, &sig_info.new, &sig_info.old);

#endif
}

bool aFunGetSignal(int signum) {
    pthread_mutex_lock(&sig_mutex);
    if (!init_sig_info) {
        pthread_mutex_unlock(&sig_mutex);
        return false;
    }

    bool re = false;
    aFuncheckSignal();

    switch (signum) {
        case SIGINT:
            re = sig_info.flat_int != 0;
            sig_info.flat_int = 0;
            break;
        case SIGTERM:
            re = sig_info.flat_term != 0;
            sig_info.flat_term = 0;
            break;
#if (defined SIGUSR1 && defined SIGUSR2)
        case SIGUSR1:
            re = sig_info.flat_u1 != 0;
            sig_info.flat_u1 = 0;
            break;
        case SIGUSR2:
            re = sig_info.flat_u2 != 0;
            sig_info.flat_u1 = 0;
            break;
#endif
        default:
            break;
    }

    pthread_mutex_unlock(&sig_mutex);
    return re;
}