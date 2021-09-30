#include <stdio.h>
#include "aFun.h"

int main() {
    aFunCoreInit("env_init-", log_pc_all, false, false, NULL, log_debug);

    af_Environment *env = makeEnvironment(grt_always);
    enableEnvironment(env);
    freeEnvironment(env);
    return 0;
}