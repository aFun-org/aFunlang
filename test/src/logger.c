#include "aFun.h"

int main(int argc, char **argv){
    jmp_buf main_buf;
    char *base_path = getExedir(1);
    if (base_path == NULL)
        goto INIT_ERROR;

    if (setjmp(main_buf) == 1)
        aFunExit(aFunExitFail);

    aFunInitInfo info = {.base_dir=base_path,
            .level=log_track,
            .log_asyn=true,
            .buf=&main_buf,
    };

    if (!aFunInit(&info)) {
INIT_ERROR:
        printf_stderr(0, "aFunlang init error.");
        aFunExit(aFunExitFail);
    }

    aFunDestruct();
    return 0;
}