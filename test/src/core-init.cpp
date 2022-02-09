#include "core-init.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto factor = aFuntool::LogFactory(std::string(".") + aFuntool::SEP + "aFunlog", true);
    auto core_logger = aFuntool::Logger(factor, "aFun-core");
    auto sys_logger = aFuntool::Logger(factor, "aFun-sys");
    auto info = aFuncore::InitInfo(factor, core_logger, sys_logger);

    if (!aFunCoreInit(&info)) {
        printf_stderr(0, "aFunlang init error.");
        aFunExitReal(EXIT_FAILURE);
    }
    return 0;
}