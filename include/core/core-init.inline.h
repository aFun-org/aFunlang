#ifndef AFUN_CORE_INIT_INLINE_H
#define AFUN_CORE_INIT_INLINE_H

#include "core-init.h"

namespace aFuncore {
    InitInfo::InitInfo(const std::string &base_dir_,
                              aFuntool::LogFactory &factor_,
                              aFuntool::Logger &core_logger_,
                              aFuntool::Logger &sys_logger_) :
        base_dir{base_dir_}, factor{factor_}, core_logger{core_logger_}, sys_logger{sys_logger_} {

    }

    void setCoreLogger(aFuntool::Logger *log) {
        aFunCoreLogger = log;
    }
}

#endif //AFUN_CORE_INIT_INLINE_H
