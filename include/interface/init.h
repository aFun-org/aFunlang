#ifndef AFUN_INIT_H
#define AFUN_INIT_H
#include "aFunlangExport.h"
#include "aFuncore.h"
#include "aFuncode.h"
#include "aFuntool.h"

namespace aFunit {
    struct InitInfo {
        aFuntool::LogFactory &factor;
        aFuntool::Logger &core_logger;
        aFuntool::Logger &code_logger;
        aFuntool::Logger &sys_logger;
        aFuntool::Logger &afun_logger;

        AFUN_INLINE InitInfo(aFuntool::LogFactory &factor_,
                             aFuntool::Logger &afun_logger_,
                             aFuntool::Logger &core_logger_,
                             aFuntool::Logger &code_logger_,
                             aFuntool::Logger &sys_logger_);
    };

    AFUN_LANG_EXPORT extern aFuntool::Logger *aFunLogger;
    AFUN_LANG_EXPORT bool aFunInit(InitInfo *info);
    AFUN_STATIC void setAFunLogger(aFuntool::Logger *log);
}

#include "init.inline.h"

#endif //AFUN_INIT_H
