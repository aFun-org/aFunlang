#include <stdio.h>
#include "aFun.h"

int main() {
    aFunCoreInit(NULL);

    af_Environment *env = makeEnvironment(grt_always);
    enableEnvironment(env);
    freeEnvironment(env);
    return 0;
}