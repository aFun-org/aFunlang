#ifndef AFUN_CORE_INIT_H
#define AFUN_CORE_INIT_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    struct InitInfo {
        aFuntool::LogFactory &factor;
        aFuntool::Logger &core_logger;
        aFuntool::Logger &sys_logger;

        AFUN_INLINE InitInfo(aFuntool::LogFactory &factor_,
                             aFuntool::Logger &core_logger_,
                             aFuntool::Logger &sys_logger_);

    };

    AFUN_CORE_EXPORT extern aFuntool::Logger *aFunCoreLogger;

    AFUN_CORE_EXPORT bool aFunCoreInit(InitInfo *info);
    AFUN_STATIC void setCoreLogger(aFuntool::Logger *log);
}

#include "core-init.inline.h"

#endif //AFUN_CORE_INIT_H