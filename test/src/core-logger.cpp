#include "core-logger.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto factor = aFuntool::LogFactory(std::string(".") + aFuntool::SEP + "aFunlog", true);
    auto logger = aFuntool::Logger(factor, "aFun");
    debugLog(&logger, "I am debug\n");
    infoLog(&logger, "I am info\n");
    warningLog(&logger, "I am warning\n");
    errorLog(&logger, "I am error\n");
    aFunExitReal(EXIT_FAILURE);
}