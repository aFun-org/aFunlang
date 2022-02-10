#include <mutex>
#include <stack>
#include <utility>
#include "tool-exit.h"
#include "tool-exception.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    class ExitManager {
    public:
        ExitManager() noexcept : exit_mutex{},  data{} {
            /* 不能将改函数改为default,
             * 否则 (implicitly deleted because its exception-specification does not match the implicit ) */
        }

        ~ExitManager() noexcept {
            runExitData();
        }

        void runExitData() {
            std::unique_lock<std::mutex> ul{exit_mutex};
            while(!data.empty()) {
                auto tmp = data.top();
                tmp.func(tmp.data);
                data.pop();
            }
        }

        void pushExitData(aFunExitFunc func, void *data_) {
            std::unique_lock<std::mutex> ul{exit_mutex};
            data.push({std::move(func), data_});
        }

        bool tryRunExitData() {
            if (!exit_mutex.try_lock())
                return false;
            std::unique_lock<std::mutex> ul{exit_mutex, std::adopt_lock};
            while(!data.empty()) {
                auto tmp = data.top();
                tmp.func(tmp.data);
                data.pop();
            }
            return true;
        }

        bool tryPushExitData(aFunExitFunc func, void *data_) {
            if (!exit_mutex.try_lock())
                return false;
            std::unique_lock<std::mutex> ul{exit_mutex, std::adopt_lock};
            data.push({std::move(func), data_});
            return true;
        }

    private:
        std::mutex exit_mutex;
        struct ExitFuncData {
            aFunExitFunc func;
            void *data;
        };
        std::stack<ExitFuncData> data;
    } manager;

    /**
     * 退出程序
     * @param exit_code 退出代码
     */
    void aFunExit(int exit_code) noexcept(false) {
        manager.runExitData();
        throw Exit(exit_code);
    }

    /**
     * 退出程序
     * @param exit_code 退出代码
     */
    [[noreturn]] void aFunExitReal(int exit_code) {
        manager.runExitData();
        exit(exit_code);
    }


    /**
     * 尝试执行退出函数
     */
    bool aFunTryExitPseudo(){
        return manager.tryRunExitData();
    }

    /**
     * 执行退出函数, 但不退出
     */
    void aFunExitPseudo(){
        manager.runExitData();
    }

    /**
     * 尝试注册退出函数, 若锁占用则返回-1
     * @param func 退出函数
     * @param data 参数
     */
    bool aFunAtExitTry(aFunExitFunc func, void *data){
        return manager.tryPushExitData(std::move(func), data);
    }

    /**
     * 注册退出函数, aFun退出函数会在atexit退出函数之前执行
     * @param func 退出函数
     * @param data 参数
     * @return
     */
    void aFunAtExit(aFunExitFunc func, void *data){
        manager.pushExitData(std::move(func), data);
    }

#ifndef AFUN_TOOL_C
}
#endif
