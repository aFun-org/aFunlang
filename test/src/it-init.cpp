#include "init.h"

int main() {
    auto factor = aFuntool::LogFactory(std::string(".") + aFuntool::SEP + "aFunlog", true);
    auto core_logger = aFuntool::Logger(factor, "aFun-core");
    auto sys_logger = aFuntool::Logger(factor, "aFun-sys");
    auto aFun_logger = aFuntool::Logger(factor, "aFun");
    auto info = aFunit::InitInfo(factor, aFun_logger, core_logger, core_logger, core_logger, sys_logger);

    if (!aFunInit(&info)) {
        aFuntool::printf_stderr(0, "aFunlang init error.");
        aFuntool::aFunExitReal(EXIT_FAILURE);
    }
    return 0;
}