#include <clocale>
#include "core-init.h"

namespace aFuncore {
    std::string log_path;
    std::string varlib_path;
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
        if (info->base_dir.empty())
            return false;

        log_path = info->base_dir + aFuntool::SEP + aFunLogDir + aFuntool::SEP;
        varlib_path = info->base_dir + aFuntool::SEP + aFunVarLibDir + aFuntool::SEP;

        std::string log = log_path + "aFunlang";
        bool re = info->factor.initLogSystem(log, info->log_asyn);
        if (re == 0)
            return false;

        static aFuntool::Logger core_logger{info->factor, "aFunlang-core", info->level};
        static aFuntool::Logger sys_logger{info->factor, "aFunlang-sys", info->level};
        aFunCoreLogger = &core_logger;
        aFuntool::setSysLogger(&sys_logger);

        debugLog(aFunCoreLogger, "aFunCore log path: %s", log_path.c_str());
        debugLog(aFunCoreLogger, "aFunCore var.lib path: %s", varlib_path.c_str());

        debugLog(aFunCoreLogger, "aFunCore init success");
        return true;
    }
}