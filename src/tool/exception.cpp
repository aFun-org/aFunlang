#include "tool-exception.h"

namespace aFuntool {
    const char *aFunException::what() {
        return message.c_str();
    }
}