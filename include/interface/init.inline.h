#ifndef AFUN_INIT_INLINE_H
#define AFUN_INIT_INLINE_H

#include "init.h"

namespace aFunit {
    InitInfo::InitInfo(aFuntool::LogFactory &factor_,
                               aFuntool::Logger &afun_logger_,
                               aFuntool::Logger &core_logger_,
                               aFuntool::Logger &code_logger_,
                               aFuntool::Logger &sys_logger_)
        : factor{factor_}, core_logger{core_logger_}, code_logger{code_logger_}, sys_logger{sys_logger_}, afun_logger{afun_logger_} {
        aFuntool::aFunAtExit(aFuntool::DlcHandle::dlcExit);
    }

    void setAFunLogger(aFuntool::Logger *log) {
        aFunLogger = log;
    }
}


#endif //AFUN_INIT_INLINE_H
