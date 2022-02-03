#ifndef AFUN_IT_INIT_INLINE_H
#define AFUN_IT_INIT_INLINE_H

#include "it-init.h"

namespace aFunit {
    inline aFunInitInfo::aFunInitInfo(const std::string &base_dir_,
                                      aFuntool::LogFactory &factor_,
                                      bool log_asyn_,
                                      aFuntool::LogLevel core_level_,
                                      aFuntool::LogLevel lang_level_) :
        InitInfo(base_dir_, factor_, log_asyn_, core_level_), lang_level{lang_level_} {

    }
}


#endif //AFUN_IT_INIT_INLINE_H
