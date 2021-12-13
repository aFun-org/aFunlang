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

    extern std::string log_path;
    extern std::string lang_path;
    extern std::string varlib_path;
    extern aFuntool::Logger *aFunCoreLogger;

    bool aFunCoreInit(InitInfo *info);
}

#endif //AFUN_INIT_HPP