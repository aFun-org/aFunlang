#ifndef AFUN_MEM_INLINE_H
#define AFUN_MEM_INLINE_H

#include <cstdlib>
#include "log.h"
#include "exit_.h"
#include "tool-logger.h"

/* 取代calloc函数 */
namespace aFuntool {
    template <typename T>
    static void *safeFree(T *ptr) {if (ptr != nullptr) free((void *)ptr); return nullptr;}

    static void *safeCalloc(size_t n, size_t size){
        void *re = calloc(n, size);
        if (re == nullptr) {
            if (SysLogger)
                fatalErrorLog(SysLogger, EXIT_FAILURE, "The memory error");
            else
                aFunExit(EXIT_FAILURE);
        }
        return re;
    }

    template <typename T>
    static void *safeCalloc(size_t n, T &t){
        void *re = calloc(n, sizeof(decltype(*t)));  // 自动推断类型
        if (re == nullptr) {
            if (SysLogger)
                fatalErrorLog(SysLogger, EXIT_FAILURE, "The memory error");
            else
                aFunExit(EXIT_FAILURE);
        }
        return re;
    }
}

#endif //AFUN_MEM_INLINE_H
