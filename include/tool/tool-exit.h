#ifndef AFUN_EXIT_H
#define AFUN_EXIT_H
#include "aFunToolExport.h"

namespace aFuntool {
    typedef void aFunExitFunc(void *);

    AFUN_TOOL_EXPORT void aFunExit(int exit_code) noexcept(false);
    [[noreturn]] AFUN_TOOL_EXPORT void aFunExitReal(int exit_code);
    AFUN_TOOL_EXPORT bool aFunTryExitPseudo();
    AFUN_TOOL_EXPORT void aFunExitPseudo();
    AFUN_TOOL_EXPORT bool aFunAtExitTry(aFunExitFunc *func, void *data);
    AFUN_TOOL_EXPORT void aFunAtExit(aFunExitFunc *func, void *data);
}

#endif //AFUN_EXIT_H
