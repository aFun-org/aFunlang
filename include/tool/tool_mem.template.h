#ifndef AFUN_TOOL_MEM_TEMPLATE_H
#define AFUN_TOOL_MEM_TEMPLATE_H
#ifdef __cplusplus

#include <cstdlib>
#include "tool_log.h"
#include "tool_exit.h"
#include "tool_logger.h"

/* 取代calloc函数 */
#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    template <typename T = void *>
    T *safeFree(T *ptr) {if (ptr != nullptr) free((void *)ptr); return nullptr;}

    template <typename T = void *>
    T *safeCalloc(size_t n, size_t size){
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
    T *safeCalloc(size_t n = 1){
        T *re = (T *)calloc(n, sizeof(T));  // 自动推断类型
        if (re == nullptr) {
            if (aFunSysLogger)
                fatalErrorLog(aFunSysLogger, EXIT_FAILURE, "The memory error");
            else
                aFunExit(EXIT_FAILURE);
        }
        return re;
    }

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_TOOL_MEM_TEMPLATE_H
