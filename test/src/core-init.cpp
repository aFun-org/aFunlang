#include "core-init.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    std::string base_path = getHomePath();
    if (base_path.empty()) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(EXIT_FAILURE);
    }

    auto factor = aFuntool::LogFactory(base_path + aFuntool::SEP + "aFunlog", true);
    auto core_logger = aFuntool::Logger(factor, "aFun-core");
    auto sys_logger = aFuntool::Logger(factor, "aFun-sys");
    aFuncore::InitInfo info {base_path, factor, core_logger, sys_logger};

    if (!aFunCoreInit(&info)) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(EXIT_FAILURE);
    }
    return 0;
}