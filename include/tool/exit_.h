#ifndef AFUN_EXIT_H
#define AFUN_EXIT_H
#include <stdlib.h>

typedef void aFunExitFunc(void *);

#define aFunExitSuccess EXIT_SUCCESS
#define aFunExitFail EXIT_FAILURE

AFUN_TOOL_EXPORT _Noreturn void aFunExit(int exit_code);
AFUN_TOOL_EXPORT int aFunTryExitPseudo(void);
AFUN_TOOL_EXPORT int aFunExitPseudo(void);
AFUN_TOOL_EXPORT int aFunAtExitTry(aFunExitFunc *func, void *data);
AFUN_TOOL_EXPORT int aFunAtExit(aFunExitFunc *func, void *data);

#endif //AFUN_EXIT_H
