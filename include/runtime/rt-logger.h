#ifndef AFUN_RT_LOGGER_H
#define AFUN_RT_LOGGER_H
#include "aFunRuntimeExport.h"
#include "aFuntool.h"

namespace aFunrt {
    AFUN_RT_EXPORT extern aFuntool::Logger *aFunRuntimeLogger;
    AFUN_STATIC void setRuntimeLogger(aFuntool::Logger *log);
}

#include "rt-logger.inline.h"
#endif //AFUN_RT_LOGGER_H
