#include "aFun.h"
#include "signal.h"
#include "main.h"

static volatile sig_atomic_t sig = 0;  // SIGINT (Windows & *nix)

static void sigFunc(int signum);
static void sigFunc(int signum) {
    if (signum != SIGINT && signum != SIGTERM)
        return;
    sig = 1;
}

bool getSignal(void) {
    int re = sig;
    if (re == 1) {
        sig = 0;
        signal(SIGINT, sigFunc);
        signal(SIGTERM, sigFunc);
        return true;
    }
    return false;
}

void signalInit(void) {
    writeDebugLog(aFunlangLogger, "aFunlang signal init");
    sig = 0;
    signal(SIGINT, sigFunc);
    signal(SIGTERM, sigFunc);
}

