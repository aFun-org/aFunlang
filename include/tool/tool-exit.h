#ifndef AFUN_EXIT_H
#define AFUN_EXIT_H
#ifdef __cplusplus

#include <functional>
#include "aFunToolExport.h"
#include "tool-macro.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    typedef std::function<void(void *)> aFunExitFunc;

    AFUN_TOOL_EXPORT void aFunExit(int exit_code) noexcept(false);
    [[noreturn]] AFUN_TOOL_EXPORT void aFunExitReal(int exit_code);
    AFUN_TOOL_EXPORT bool aFunTryExitPseudo();
    AFUN_TOOL_EXPORT void aFunExitPseudo();

    AFUN_TOOL_EXPORT bool aFunAtExitTry(aFunExitFunc func, void *data);
    AFUN_TOOL_EXPORT void aFunAtExit(aFunExitFunc func, void *data);

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_EXIT_H
