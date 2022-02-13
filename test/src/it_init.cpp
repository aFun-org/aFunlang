#include "init.h"

int main() {
    if (!aFunit::aFunInit()) {
        aFuntool::printf_stderr(0, "aFunlang init error.");
        aFuntool::aFunExitReal(EXIT_FAILURE);
    }
    return 0;
}