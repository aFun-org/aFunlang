#ifndef AFUN_IT_INIT_H
#define AFUN_IT_INIT_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFunit {
    struct aFunInitInfo : public aFuncore::InitInfo {
        aFuntool::Logger &afun_logger;

        AFUN_INLINE aFunInitInfo(const std::string &base_dir_,
                            aFuntool::LogFactory &factor_,
                            aFuntool::Logger &afun_logger_,
                            aFuntool::Logger &core_logger_,
                            aFuntool::Logger &sys_logger_);
    };

    AFUN_LANG_EXPORT extern aFuntool::Logger *aFunLogger;
    AFUN_LANG_EXPORT bool aFunInit(aFunInitInfo *info);
    AFUN_STATIC void setAFunLogger(aFuntool::Logger *log);
}

#include "it-init.inline.h"

#endif //AFUN_IT_INIT_H
