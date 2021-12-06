#include <cstdio>
#include "aFun.hpp"

int main() {
    aFunCoreInit(nullptr);

    af_Environment *env = makeEnvironment(grt_always);
    enableEnvironment(env);
    freeEnvironment(env);
    return 0;
}