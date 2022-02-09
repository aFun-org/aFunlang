#include "it-init.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto factor = aFuntool::LogFactory(std::string(".") + aFuntool::SEP + "aFunlog", true);
    auto core_logger = aFuntool::Logger(factor, "aFun-core");
    auto sys_logger = aFuntool::Logger(factor, "aFun-sys");
    auto aFun_logger = aFuntool::Logger(factor, "aFun");
    auto info = aFunit::aFunInitInfo(factor, aFun_logger, core_logger, sys_logger);

    if (!aFunInit(&info)) {
        printf_stderr(0, "aFunlang init error.");
        aFunExitReal(EXIT_FAILURE);
    }
    return 0;
}