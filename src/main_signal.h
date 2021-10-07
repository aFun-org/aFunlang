#ifndef AFUN_MAIN_SIGNAL_H
#define AFUN_MAIN_SIGNAL_H
#include "main.h"

typedef struct SignalInfo SignalInfo;
struct SignalInfo {
    void *sig_int;
    void *sig_term;
};

bool getSignal(void);
void signalInit(SignalInfo *si);
void signalRecover(SignalInfo *si);

#endif //AFUN_MAIN_SIGNAL_H
