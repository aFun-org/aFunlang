#include "tool.h"
#include "exit_.h"
#include "mutex"

using namespace aFuntool;

static const int exit_func_size = 1024;
static std::mutex exit_mutex;
struct ExitFuncData {
    aFunExitFunc *func;
    void *data;
} exit_func[exit_func_size];

/**
 * 退出程序
 * @param exit_code 退出代码
 */
[[ noreturn ]] void aFuntool::aFunExit(int exit_code) {
    std::unique_lock<std::mutex> ul{exit_mutex};
    for (int i = exit_func_size - 1; i >= 0; i--) {
        if (exit_func[i].func != nullptr)
            exit_func[i].func(exit_func[i].data);
    }
    ul.unlock();
    exit(exit_code);
}

/**
 * 尝试执行退出函数
 */
int aFuntool::aFunTryExitPseudo() {
    if (exit_mutex.try_lock()) {
        std::unique_lock<std::mutex> ul{exit_mutex, std::adopt_lock};
        for (int i = exit_func_size - 1; i >= 0; i--) {
            if (exit_func[i].func != nullptr)
                exit_func[i].func(exit_func[i].data);
            exit_func[i].func = nullptr;
            exit_func[i].data = nullptr;
        }
        return 1;
    }
    return 0;
}

/**
 * 执行退出函数, 但不退出
 */
int aFuntool::aFunExitPseudo() {
    std::unique_lock<std::mutex> ul{exit_mutex};
    for (int i = exit_func_size - 1; i >= 0; i--) {
        if (exit_func[i].func != nullptr)
            exit_func[i].func(exit_func[i].data);
        exit_func[i].func = nullptr;
        exit_func[i].data = nullptr;
    }
    return 0;
}

/**
 * 尝试注册退出函数, 若锁占用则返回-1
 * @param func 退出函数
 * @param data 参数
 */
int aFuntool::aFunAtExitTry(aFunExitFunc *func, void *data) {
    if (exit_mutex.try_lock()) {
        std::unique_lock<std::mutex> ul{exit_mutex, std::adopt_lock};
        struct ExitFuncData *tmp = exit_func;
        int count = 0;
        for(NULL; tmp->func != nullptr; tmp++, count++) {
            if (count >= exit_func_size) {
                return -1;
            }
        }
        tmp->func = func;
        tmp->data = data;
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
    std::unique_lock<std::mutex> ul{exit_mutex};
    struct ExitFuncData *tmp = exit_func;
    int count = 0;
    for(NULL; tmp->func != nullptr; tmp++, count++) {
        if (count >= exit_func_size) {
            return -1;
        }
    }
    tmp->func = func;
    tmp->data = data;
    return count;
}
