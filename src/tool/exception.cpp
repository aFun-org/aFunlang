#include "tool-exception.h"

namespace aFuntool {
    const char *aFunException::what() const noexcept {
        return message.c_str();
    }
}