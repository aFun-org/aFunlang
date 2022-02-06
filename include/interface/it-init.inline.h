#ifndef AFUN_IT_INIT_INLINE_H
#define AFUN_IT_INIT_INLINE_H

#include "it-init.h"

namespace aFunit {
    inline aFunInitInfo::aFunInitInfo(const std::string &base_dir_,
                                      aFuntool::LogFactory &factor_,
                                      aFuntool::Logger &afun_logger_,
                                      aFuntool::Logger &core_logger_,
                                      aFuntool::Logger &sys_logger_) :
        InitInfo(base_dir_, factor_, sys_logger_, sys_logger_), afun_logger{afun_logger_} {

    }

    static void setAFunLogger(aFuntool::Logger *log) {
        aFunLogger = log;
    }
}


#endif //AFUN_IT_INIT_INLINE_H
