#include "rt-init.h"

namespace aFunrt {
    aFuntool::Logger *aFunLogger = nullptr;

    bool aFunInit(aFunInitInfo *info) {
        if (!aFuncore::aFunCoreInit(info))
            return false;
        static aFuntool::Logger logger{info->factor, "aFunlang", info->lang_level};
        aFunLogger = &logger;
        return true;
    }

}