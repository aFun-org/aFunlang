#include <clocale>
#include "init.hpp"
using namespace aFuncore;
using namespace aFuntool;

namespace aFuncore {
    std::string log_path;
    std::string lang_path;
    std::string varlib_path;
    aFuntool::Logger *aFunCoreLogger;
};

bool aFuncore::aFunCoreInit(aFuncore::InitInfo *info) {
    if (info == nullptr) {
        static InitInfo info_default = {.base_dir=".",
                                        .log_asyn=true,
                                        .level=log_info};
        info = &info_default;
    }

    getEndian();
    if (setlocale(LC_ALL, "") == nullptr)
        return false;
    if (info->base_dir.empty())
        return false;

    log_path = info->base_dir + SEP + aFunLogDir + SEP;
    lang_path = info->base_dir + SEP + aFunLangDir + SEP;
    varlib_path = info->base_dir + SEP + aFunVarLibDir + SEP;

    std::string log = log_path + "aFunlang";
    bool re = log_factory.initLogSystem(log, info->log_asyn);
    if (re == 0)
        return false;

    static aFuntool::Logger logger {"aFunlang-core", info->level};
    aFuncore::aFunCoreLogger = &logger;

    debugLog(aFunCoreLogger, "aFunCore log path: %s", log_path.c_str());
    debugLog(aFunCoreLogger, "aFunCore var.lib path: %s", varlib_path.c_str());
    debugLog(aFunCoreLogger, "aFunCore lang path: %s", lang_path.c_str());

    char LANG_path[218] = {0};
    snprintf(LANG_path, 218, "%sLANG", lang_path.c_str());

    FILE *LANG_file = fileOpen(LANG_path, "r");
    if (LANG_file != nullptr) {
        char LANG[100] = {0};
        fgets(LANG, 100, LANG_file);
        if (LANG[strlen(LANG) - 1] == '\n')
            LANG[strlen(LANG) - 1] = NUL;  // 去除`\n`
        debugLog(aFunCoreLogger, "language = %s", LANG);

        char LANG_lib[218] = {0};
        std::string tmp = std::string("%s") + SHARED_PREFIX + "%s" + SHARED_SUFFIX;
        snprintf(LANG_lib, 218, tmp.c_str(), lang_path.c_str(), LANG);
        if (HT_initaFunGetText(LANG_lib) == 0)
            debugLog(aFunCoreLogger, "aFunCore lang init success: %s", LANG_lib);
        else
            debugLog(aFunCoreLogger, "aFunCore lang init failed: %s", LANG_lib);
        fileClose(LANG_file);
    } else
        HT_initaFunGetText(nullptr);
    debugLog(aFunCoreLogger, "aFunCore init success");
    return true;
}