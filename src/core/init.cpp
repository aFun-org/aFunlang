#include <clocale>
#include "init.h"
using namespace aFuncore;
using namespace aFuntool;

namespace aFuncore {
    std::string log_path;
    std::string varlib_path;
    aFuntool::Logger *aFunCoreLogger;
};

/**
 * 初始化程序
 * @param info 初始化信息
 * @return 是否初始化成功
 */
bool aFuncore::aFunCoreInit(aFuncore::InitInfo *info) {
    if (info == nullptr)
        return false;

    getEndian();
    if (setlocale(LC_ALL, "") == nullptr)
        return false;
    if (info->base_dir.empty())
        return false;

    log_path = info->base_dir + SEP + aFunLogDir + SEP;
    varlib_path = info->base_dir + SEP + aFunVarLibDir + SEP;

    std::string log = log_path + "aFunlang";
    bool re = info->factor.initLogSystem(log, info->log_asyn);
    if (re == 0)
        return false;

    static aFuntool::Logger logger {info->factor, "aFunlang-core", info->level};
    aFuncore::aFunCoreLogger = &logger;

    debugLog(aFunCoreLogger, "aFunCore log path: %s", log_path.c_str());
    debugLog(aFunCoreLogger, "aFunCore var.lib path: %s", varlib_path.c_str());

    debugLog(aFunCoreLogger, "aFunCore init success");
    return true;
}