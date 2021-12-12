#include "init.hpp"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    std::string base_path = getExedir(1);
    if (base_path.empty()) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(EXIT_FAILURE);
    }

    aFuncore::InitInfo info = {.base_dir=base_path,
                               .log_asyn=true,
                               .level=log_debug,
    };

    if (!aFunCoreInit(&info)) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(EXIT_FAILURE);
    }
    return 0;
}