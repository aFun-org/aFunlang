#ifndef AFUN_EXIT_H
#define AFUN_EXIT_H
#include <cstdlib>

typedef void aFunExitFunc(void *);

#define aFunExitSuccess EXIT_SUCCESS
#define aFunExitFail EXIT_FAILURE

[[ noreturn ]] AFUN_TOOL_EXPORT void aFunExit(int exit_code);
AFUN_TOOL_EXPORT int aFunTryExitPseudo();
AFUN_TOOL_EXPORT int aFunExitPseudo();
AFUN_TOOL_EXPORT int aFunAtExitTry(aFunExitFunc *func, void *data);
AFUN_TOOL_EXPORT int aFunAtExit(aFunExitFunc *func, void *data);

#endif //AFUN_EXIT_H
