#ifndef AFUN_RT_EXCEPTION_H
#define AFUN_RT_EXCEPTION_H
#include "aFuntool.h"

namespace aFunrt {
    class aFunrtException : public aFuntool::aFunException {
    public:
        inline explicit aFunrtException(const std::string &msg);
    };

    class readerFileOpenError : public aFunrtException {
    public:
        inline explicit readerFileOpenError(const aFuntool::FilePath &file);
    };

}

#include "rt-exception.inline.h"

#endif //AFUN_RT_EXCEPTION_H
