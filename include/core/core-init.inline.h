#ifndef AFUN_CORE_INIT_INLINE_H
#define AFUN_CORE_INIT_INLINE_H

#include "core-init.h"

namespace aFuncore {
    inline InitInfo::InitInfo(const std::string &base_dir_,
                              aFuntool::LogFactory &factor_,
                              bool log_asyn_,
                              aFuntool::LogLevel level_) :
        base_dir{base_dir_}, factor{factor_}, log_asyn{log_asyn_}, level{level_} {

    }
}

#endif //AFUN_CORE_INIT_INLINE_H
