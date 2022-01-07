#include "tool.h"
#include "exit_.h"
#include "pthread.h"
using namespace aFuntool;

static const int exit_func_size = 1024;
static pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;
struct ExitFuncData {
    aFunExitFunc *func;
    void *data;
} exit_func[exit_func_size];

/**
 * 退出程序
 * @param exit_code 退出代码
 */
[[ noreturn ]] void aFuntool::aFunExit(int exit_code) {
    if (pthread_mutex_trylock(&exit_mutex) == 0) {
        int count = 0;
        for (struct ExitFuncData *tmp = exit_func; tmp->func != nullptr && count < exit_func_size; tmp++, count++)
            tmp->func(tmp->data);
        pthread_mutex_unlock(&exit_mutex);
    }
    exit(exit_code);
}

/**
 * 尝试执行退出函数
 */
int aFuntool::aFunTryExitPseudo() {
    if (pthread_mutex_trylock(&exit_mutex) == 0) {
        int count = 0;
        for (struct ExitFuncData *tmp = exit_func; tmp->func != nullptr && count < exit_func_size; tmp++, count++) {
            tmp->func(tmp->data);
            tmp->data = nullptr;
            tmp->func = nullptr;
        }
        pthread_mutex_unlock(&exit_mutex);
        return 1;
    }
    return 0;
}

/**
 * 执行退出函数, 但不退出
 */
int aFuntool::aFunExitPseudo() {
    if (pthread_mutex_lock(&exit_mutex) == 0) {
        int count = 0;
        for (struct ExitFuncData *tmp = exit_func; tmp->func != nullptr && count < exit_func_size; tmp++, count++) {
            tmp->func(tmp->data);
            tmp->data = nullptr;
            tmp->func = nullptr;
        }
        pthread_mutex_unlock(&exit_mutex);
        return 1;
    }
    return 0;
}

/**
 * 尝试注册退出函数, 若锁占用则返回-1
 * @param func 退出函数
 * @param data 参数
 */
int aFuntool::aFunAtExitTry(aFunExitFunc *func, void *data) {
    if (pthread_mutex_trylock(&exit_mutex) == 0) {
        struct ExitFuncData *tmp = exit_func;
        int count = 0;
        for(NULL; tmp->func != nullptr; tmp++, count++) {
            if (count >= exit_func_size) {
                pthread_mutex_unlock(&exit_mutex);
                return -1;
            }
        }
        tmp->func = func;
        tmp->data = data;
        pthread_mutex_unlock(&exit_mutex);
        return count;
    }
    return -1;
}

/**
 * 注册退出函数, aFun退出函数会在atexit退出函数之前执行
 * @param func 退出函数
 * @param data 参数
 * @return
 */
int aFuntool::aFunAtExit(aFunExitFunc *func, void *data) {
    if (pthread_mutex_lock(&exit_mutex) == 0) {
        struct ExitFuncData *tmp = exit_func;
        int count = 0;
        for(NULL; tmp->func != nullptr; tmp++, count++) {
            if (count >= exit_func_size) {
                pthread_mutex_unlock(&exit_mutex);
                return -1;
            }
        }
        tmp->func = func;
        tmp->data = data;
        pthread_mutex_unlock(&exit_mutex);
        return count;
    }
    return -1;
}
