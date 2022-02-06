#ifndef AFUN_MEM_TEMPLATE_H
#define AFUN_MEM_TEMPLATE_H

#include <cstdlib>
#include "log.h"
#include "tool-exit.h"
#include "tool-logger.h"

/* 取代calloc函数 */
namespace aFuntool {
    template <typename T = void *>
    static T *safeFree(T *ptr) {if (ptr != nullptr) free((void *)ptr); return nullptr;}

    template <typename T = void *>
    static T *safeCalloc(size_t n, size_t size){
        T *re = (T *)calloc(n, size);
        if (re == nullptr) {
            if (aFunSysLogger)
                fatalErrorLog(aFunSysLogger, EXIT_FAILURE, "The memory error");
            else
                aFunExit(EXIT_FAILURE);
        }
        return re;
    }

    template <typename T = void *>
    static T *safeCalloc(size_t n = 1){
        T *re = (T *)calloc(n, sizeof(T));  // 自动推断类型
        if (re == nullptr) {
            if (aFunSysLogger)
                fatalErrorLog(aFunSysLogger, EXIT_FAILURE, "The memory error");
            else
                aFunExit(EXIT_FAILURE);
        }
        return re;
    }
}

#endif //AFUN_MEM_TEMPLATE_H
