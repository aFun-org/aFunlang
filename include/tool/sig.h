#ifndef AFUN_SIG_H
#define AFUN_SIG_H
#include <signal.h>

/* 信号处理工具 */
typedef int vsignal;
typedef struct SignalTag SignalTag;
struct SignalTag{
    volatile vsignal signum;  // 信号
    volatile enum SignalType{
        signal_reset=0,  // 没有信号
        signal_appear,  // 信号未被处理
    } status;
};

extern volatile struct SignalTag signal_tag;  // 在tool.c中定义
void afSignalHandler(int signum);

#endif //AFUN_SIG_H
