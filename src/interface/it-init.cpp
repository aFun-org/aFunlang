#include "it-init.h"

namespace aFunit {
    aFuntool::Logger *aFunLogger = nullptr;

    bool aFunInit(aFunInitInfo *info) {
        if (!aFuncore::aFunCoreInit(info))
            return false;
        setAFunLogger(&info->afun_logger);
        return true;
    }

}