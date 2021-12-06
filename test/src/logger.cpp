#include "aFun.hpp"

int main(int argc, char **argv){
    jmp_buf main_buf;
    char *base_path = getExedir(1);
    if (base_path == nullptr) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(aFunExitFail);
    }

    if (setjmp(main_buf) == 1)
        aFunExit(aFunExitFail);

    aFunInitInfo info = {.base_dir=base_path,
            .log_asyn=true,
            .buf=&main_buf,
            .level=log_track,
    };

    printLogSystemInfo();
    if (!aFunInit(&info)) {
        printf_stderr(0, "aFunlang init error.");
        aFunExit(aFunExitFail);
    }

    aFunDestruct();
    return 0;
}