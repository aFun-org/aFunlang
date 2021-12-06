#include "tool.hpp"
#include "pthread.h"

#define EXIT_FUNC_SIZE (1024)

static pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;
struct ExitFuncData {
    aFunExitFunc *func;
    void *data;
} exit_func[EXIT_FUNC_SIZE];

[[ noreturn ]] void aFunExit(int exit_code) {
    if (pthread_mutex_trylock(&exit_mutex) == 0) {
        int count = 0;
        for (struct ExitFuncData *tmp = exit_func; tmp->func != nullptr && count < EXIT_FUNC_SIZE; tmp++, count++)
            tmp->func(tmp->data);
        pthread_mutex_unlock(&exit_mutex);
    }
    exit(exit_code);
}

int aFunTryExitPseudo() {
    if (pthread_mutex_trylock(&exit_mutex) == 0) {
        int count = 0;
        for (struct ExitFuncData *tmp = exit_func; tmp->func != nullptr && count < EXIT_FUNC_SIZE; tmp++, count++) {
            tmp->func(tmp->data);
            tmp->data = nullptr;
            tmp->func = nullptr;
        }
        pthread_mutex_unlock(&exit_mutex);
        return 1;
    }
    return 0;
}

int aFunExitPseudo(void) {
    if (pthread_mutex_lock(&exit_mutex) == 0) {
        int count = 0;
        for (struct ExitFuncData *tmp = exit_func; tmp->func != nullptr && count < EXIT_FUNC_SIZE; tmp++, count++) {
            tmp->func(tmp->data);
            tmp->data = nullptr;
            tmp->func = nullptr;
        }
        pthread_mutex_unlock(&exit_mutex);
        return 1;
    }
    return 0;
}

int aFunAtExitTry(aFunExitFunc *func, void *data) {
    if (pthread_mutex_trylock(&exit_mutex) == 0) {
        struct ExitFuncData *tmp = exit_func;
        int count = 0;
        for(NULL; tmp->func != nullptr; tmp++, count++) {
            if (count >= EXIT_FUNC_SIZE) {
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

int aFunAtExit(aFunExitFunc *func, void *data) {
    if (pthread_mutex_lock(&exit_mutex) == 0) {
        struct ExitFuncData *tmp = exit_func;
        int count = 0;
        for(NULL; tmp->func != nullptr; tmp++, count++) {
            if (count >= EXIT_FUNC_SIZE) {
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
