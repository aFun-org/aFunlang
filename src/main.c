#include <stdio.h>
#include "aFun.h"

int main() {
    aFunInit();
    printf("Hello World\n");

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

    {  // 正常程序
        af_Code *bt1 = makeLiteralCode("data", "func", ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object", 0, 1, NULL);

        af_Code *bt3 = makeLiteralCode("data2", "func", 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);
        af_Code *bt6 = makeVariableCode("object", 0, 1, NULL);

        connectCode(&bt1, bt2);
        connectCode(&bt3, bt4);
        connectCode(&bt4, bt6);

        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
    }

    {  // 测试错误 (object2 Var not found)
        af_Code *bt1 = makeLiteralCode("data", "func", ',', 0, "Unknow");
        af_Code *bt2 = makeVariableCode("object2", 0, 1, NULL);

        af_Code *bt3 = makeLiteralCode("data2", "func", 0, 0, NULL);
        af_Code *bt4 = makeVariableCode("global", 0, 1, NULL);
        af_Code *bt6 = makeVariableCode("object", 0, 1, NULL);

        connectCode(&bt1, bt2);
        connectCode(&bt3, bt4);
        connectCode(&bt4, bt6);

        af_Code *bt5 = makeBlockCode(curly, bt3, 0, 1, NULL, NULL);
        connectCode(&bt2, bt5);

        iterCode(bt1, env);
        freeAllCode(bt1);
    }

    freeEnvironment(env);
    return 0;
}