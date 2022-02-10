#include "tool-exception.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    const char *aFunException::what() const noexcept {
        return message.c_str();
    }

#ifndef AFUN_TOOL_C
}
#endif
