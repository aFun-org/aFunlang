#include "aFunCore.hpp"
#include "__sig.hpp"
#include "pthread.h"

typedef void (*HandleFunc) (int);
struct af_SignalInfo {
    HandleFunc sig_int;
    HandleFunc sig_term;

    volatile sig_atomic_t flat_int;  // SIGUSR1 (*nix)
    volatile sig_atomic_t flat_term;  // SIGUSR1 (*nix)

#ifndef aFunWIN32_NO_CYGWIN
    volatile sig_atomic_t flat_u1;  // SIGUSR1 (*nix)
    volatile sig_atomic_t flat_u2;  // SIGUSR1 (*nix)

    HandleFunc sig_u1;
    HandleFunc sig_u2;
    sigset_t new_sigset;
    sigset_t old_sigset;
#endif
};

static void aFuncheckSignal();
typedef struct af_SignalInfo af_SignalInfo;
af_SignalInfo sig_info;
bool init_sig_info = false;
pthread_mutex_t sig_mutex = PTHREAD_MUTEX_INITIALIZER;


#ifndef aFunWIN32_NO_CYGWIN
static void aFunSigFunc(int signum);
static void aFunSigFunc(int signum) {
    switch (signum) {
        case SIGINT:
            sig_info.flat_int = 1;
            break;
        case SIGTERM:
            sig_info.flat_term = 1;
            break;
        case SIGUSR1:
            sig_info.flat_u1 = 1;
            break;
        case SIGUSR2:
            sig_info.flat_u2 = 1;
            break;
        default:
            break;
    }
}
#endif

void aFunSignalInit() {
    pthread_mutex_lock(&sig_mutex);
    if (init_sig_info) {
        pthread_mutex_unlock(&sig_mutex);
        writeWarningLog(aFunCoreLogger, "Double signal init");
        return;
    }

    writeDebugLog(aFunCoreLogger, "Signal init");
    init_sig_info = true;
#ifdef aFunWIN32_NO_CYGWIN
    sig_info.sig_int = signal(SIGINT, SIG_IGN);
    sig_info.sig_term = signal(SIGTERM, SIG_IGN);
    stdio_signal_init(true);
#else
    sigemptyset(&sig_info.old_sigset);
    sigemptyset(&sig_info.new_sigset);
    sigaddset(&sig_info.new_sigset, SIGINT);
    sigaddset(&sig_info.new_sigset, SIGTERM);
    sigaddset(&sig_info.new_sigset, SIGUSR1);
    sigaddset(&sig_info.new_sigset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sig_info.new_sigset, &sig_info.old_sigset);

    sig_info.sig_int = signal(SIGINT, aFunSigFunc);
    sig_info.sig_term = signal(SIGTERM, aFunSigFunc);
    sig_info.sig_u1 = signal(SIGUSR1, aFunSigFunc);
    sig_info.sig_u2 = signal(SIGUSR2, aFunSigFunc);

    sig_info.flat_int = 0;
    sig_info.flat_term = 0;
    sig_info.flat_u1 = 0;
    sig_info.flat_u2 = 0;
#endif
    pthread_mutex_unlock(&sig_mutex);
}

void aFunSignalRecover() {
    pthread_mutex_lock(&sig_mutex);
    if (!init_sig_info) {
        pthread_mutex_unlock(&sig_mutex);
        writeWarningLog(aFunCoreLogger, "Signal recover before init");
        return;
    }

    HandleFunc re;
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
    stdio_signal_init(false);
#else
    if (sig_info.sig_u1 != SIG_ERR) {
        re = signal(SIGUSR1, sig_info.sig_u1);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGUSR1 recover fail");
    }

    if (sig_info.sig_u2 != SIG_ERR) {
        re = signal(SIGUSR2, sig_info.sig_u2);
        assertWarningLog(re != SIG_ERR, aFunCoreLogger, "SIGUSR2 recover fail");
    }
    sigprocmask(SIG_SETMASK, &sig_info.old_sigset, nullptr);
#endif
    pthread_mutex_unlock(&sig_mutex);
}

static void aFuncheckSignal() {
    /* 该函数是内部函数, 不负责加锁 */
#ifdef aFunWIN32_NO_CYGWIN
    if (stdio_check_signal())
        sig_info.flat_int = 1;
#else
    for (int i = 0; i < 4; i++) {
        // 标准: sigprocmask 执行返回前必有一个未递达(如果有)的信号递达
        // 循环 count 次, 确保信号都递达
        signal(SIGINT, aFunSigFunc);
        signal(SIGTERM, aFunSigFunc);
        signal(SIGUSR1, aFunSigFunc);
        signal(SIGUSR2, aFunSigFunc);
        sigprocmask(SIG_SETMASK, &sig_info.old_sigset, nullptr);  // 让信号递达
    }
    sigprocmask(SIG_BLOCK, &sig_info.new_sigset, &sig_info.old_sigset);
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
            re = sig_info.flat_int == 1;
            sig_info.flat_int = 0;
            break;
        case SIGTERM:
            re = sig_info.flat_term == 1;
            sig_info.flat_term = 0;
            break;
#ifndef aFunWIN32_NO_CYGWIN
        case SIGUSR1:
            re = sig_info.flat_u1 == 1;
            sig_info.flat_u1 = 0;
            break;
        case SIGUSR2:
            re = sig_info.flat_u2 == 1;
            sig_info.flat_u1 = 0;
            break;
#endif
        default:
            break;
    }

    pthread_mutex_unlock(&sig_mutex);
    return re;
}