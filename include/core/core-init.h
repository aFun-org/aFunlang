#ifndef AFUN_CORE_INIT_H
#define AFUN_CORE_INIT_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    struct InitInfo {
        const std::string &base_dir;
        aFuntool::LogFactory &factor;
        bool log_asyn;
        aFuntool::LogLevel level;

        inline InitInfo(const std::string &base_dir_,
                        aFuntool::LogFactory &factor_,
                        bool log_asyn_,
                        aFuntool::LogLevel level_);

    };

    AFUN_CORE_EXPORT extern std::string log_path;
    AFUN_CORE_EXPORT extern std::string lang_path;
    AFUN_CORE_EXPORT extern std::string varlib_path;
    AFUN_CORE_EXPORT extern aFuntool::Logger *aFunCoreLogger;

    AFUN_CORE_EXPORT bool aFunCoreInit(InitInfo *info);
}

#include "core-init.inline.h"

#endif //AFUN_CORE_INIT_H