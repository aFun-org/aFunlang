#ifndef AFUN_INIT_HPP
#define AFUN_INIT_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    struct InitInfo {
        const std::string &base_dir;

        bool log_asyn;
        aFuntool::LogLevel level;
    };

    AFUN_CORE_EXPORT extern std::string log_path;
    AFUN_CORE_EXPORT extern std::string lang_path;
    AFUN_CORE_EXPORT extern std::string varlib_path;
    AFUN_CORE_EXPORT extern aFuntool::Logger *aFunCoreLogger;

    AFUN_CORE_EXPORT bool aFunCoreInit(InitInfo *info);
}

#endif //AFUN_INIT_HPP