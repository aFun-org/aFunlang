#ifndef AFUN_IT_INIT_H
#define AFUN_IT_INIT_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFunit {
    struct aFunInitInfo : public aFuncore::InitInfo {
        aFuntool::LogLevel lang_level;

        inline aFunInitInfo(const std::string &base_dir_,
                           aFuntool::LogFactory &factor_,
                           bool log_asyn_,
                           aFuntool::LogLevel core_level_,
                           aFuntool::LogLevel lang_level_);
    };

    AFUN_LANG_EXPORT extern aFuntool::Logger *aFunLogger;
    AFUN_LANG_EXPORT bool aFunInit(aFunInitInfo *info);
}

#include "it-init.inline.h"

#endif //AFUN_IT_INIT_H
