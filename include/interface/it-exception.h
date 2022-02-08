#ifndef AFUN_IT_EXCEPTION_H
#define AFUN_IT_EXCEPTION_H
#include "aFuntool.h"

namespace aFunit {
    class aFunitException : public aFuntool::aFunException {
    public:
        AFUN_INLINE explicit aFunitException(const std::string &msg);
    };

    class readerFileOpenError : public aFunitException {
    public:
        AFUN_INLINE explicit readerFileOpenError(const aFuntool::FilePath &file);
    };

}

#include "it-exception.inline.h"

#endif //AFUN_IT_EXCEPTION_H
