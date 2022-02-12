#ifndef AFUN_CORE_LOGGER_INLINE_H
#define AFUN_CORE_LOGGER_INLINE_H

#include "core-logger.h"

namespace aFuncore {
    void setCoreLogger(aFuntool::Logger *log) {
        aFunCoreLogger = log;
    }
}

#endif //AFUN_CORE_LOGGER_INLINE_H
