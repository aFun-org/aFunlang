#ifndef AFUN___MONITOR_HPP
#define AFUN___MONITOR_HPP
#include "pthread.h"
#include "macro.hpp"

typedef struct af_Monitor af_Monitor;
struct af_Monitor {
    struct af_Environment *env;
    pthread_t thread;
    bool exit;  // 外部通知守护线程结束
    pthread_mutex_t lock;
    pthread_cond_t cond;
};


#include "__env.hpp"
AFUN_CORE_NO_EXPORT af_Monitor *makeMonitor(af_Environment *env);
AFUN_CORE_NO_EXPORT void freeMonitor(af_Environment *env);

#endif //AFUN___MONITOR_HPP
