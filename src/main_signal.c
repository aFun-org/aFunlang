#include "aFun.h"
#include "signal.h"
#include "main.h"
#include "main_signal.h"

static volatile sig_atomic_t sig = 0;  // SIGINT (Windows & *nix)

static void sigFunc(int signum);
static void sigFunc(int signum) {
    if (signum != SIGINT && signum != SIGTERM)
        return;
    sig = 1;
}

bool getSignal(void) {
    if (sig == 1 || aFunGetSignal(SIGINT) || aFunGetSignal(SIGTERM)) {
        sig = 0;
        signal(SIGINT, sigFunc);
        signal(SIGTERM, sigFunc);
        return true;
    }
    return false;
}

void signalInit(SignalInfo *si) {
    writeDebugLog(aFunlangLogger, "aFunlang signal init");
    sig = 0;
    si->sig_int = signal(SIGINT, sigFunc);
    si->sig_term = signal(SIGTERM, sigFunc);
}

void signalRecover(SignalInfo *si) {
    writeDebugLog(aFunlangLogger, "aFunlang signal init");
    sig = 0;
    if (si->sig_int != SIG_ERR)
        signal(SIGINT, si->sig_int);
    if (si->sig_term != SIG_ERR)
        signal(SIGTERM, si->sig_term);
}
