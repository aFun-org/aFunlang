#ifndef AFUN_CORE_LOGGER_H
#define AFUN_CORE_LOGGER_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    AFUN_CORE_EXPORT extern aFuntool::Logger *aFunCoreLogger;
    AFUN_STATIC void setCoreLogger(aFuntool::Logger *log);
}

#include "core-logger.inline.h"

#endif //AFUN_CORE_LOGGER_H