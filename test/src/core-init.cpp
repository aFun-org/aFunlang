#include "core-init.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    std::string base_path = getExedir(1);
    if (base_path.empty()) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(EXIT_FAILURE);
    }

    aFuntool::LogFactory factor {};
    aFuncore::InitInfo info {base_path, factor, true, log_debug};

    if (!aFunCoreInit(&info)) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(EXIT_FAILURE);
    }
    return 0;
}