#include <stdio.h>
#include "aFun.h"

int main() {
    aFunInit();

    af_Environment *env = makeEnvironment();
    addVarToProtectVarSpace(makeVar("global", 3, 3, 3,
                                    makeObject("global", 0, false, true, NULL, NULL, env)),
                            env);
    addVarToProtectVarSpace(makeVar("object", 3, 3, 3,
                                    makeObject("object", 0, false, true, NULL, NULL, env)),
                            env);
    if (!enableEnvironment(env)) {
        fprintf(stderr, "Enable Error.\n");
        exit(EXIT_FAILURE);
    }

    freeEnvironment(env);
    return 0;
}