#ifndef AFUN_CODE_LOGGER_H
#define AFUN_CODE_LOGGER_H
#include "aFunCodeExport.h"
#include "aFuntool.h"

namespace aFuncode {
    AFUN_CODE_EXPORT extern aFuntool::Logger *aFunCodeLogger;
    AFUN_INLINE void setCodeLogger(aFuntool::Logger *log);
}

#include "code-logger.inline.h"

#endif //AFUN_CODE_LOGGER_H
