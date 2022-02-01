#ifndef AFUN_RT_EXCEPTION_INLINE_H
#define AFUN_RT_EXCEPTION_INLINE_H

#include "rt-exception.h"

namespace aFunrt {
    inline aFunrtException::aFunrtException(const std::string &msg) : aFunException{msg} {

    }

    inline readerFileOpenError::readerFileOpenError(const aFuntool::FilePath &file)
        : aFunrtException("Reader cannot open file: " + file) {

    }
}

#endif //AFUN_RT_EXCEPTION_INLINE_H
