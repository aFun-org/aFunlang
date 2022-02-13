#ifndef AFUN_RT_LOGGER_INLINE_H
#define AFUN_RT_LOGGER_INLINE_H

#include "rt-logger.h"

namespace aFunrt {
    void setRuntimeLogger(aFuntool::Logger *log) {
        aFunRuntimeLogger = log;
    }
}

#endif //AFUN_RT_LOGGER_INLINE_H
