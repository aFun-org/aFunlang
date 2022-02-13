#include "init.h"

namespace aFunit {
    aFuntool::Logger *aFunLogger = nullptr;

    bool aFunInit() {
        aFuntool::getEndian();
        if (setlocale(LC_ALL, "") == nullptr)
            return false;
        return true;
    }

}