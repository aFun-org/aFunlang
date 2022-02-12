#include <clocale>
#include "core-init.h"
#include "core-logger.h"

namespace aFuncore {
    aFuntool::Logger *aFunCoreLogger = nullptr;

    /**
     * 初始化程序
     * @param info 初始化信息
     * @return 是否初始化成功
     */
    bool aFunCoreInit(InitInfo *info){
        if (info == nullptr)
            return false;

        aFuntool::getEndian();
        if (setlocale(LC_ALL, "") == nullptr)
            return false;

        setCoreLogger(&info->core_logger);
        aFuntool::setSysLogger(&info->sys_logger);

        debugLog(aFunCoreLogger, "aFunCore init success");
        return true;
    }
}