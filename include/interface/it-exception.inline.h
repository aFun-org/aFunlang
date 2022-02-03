﻿#ifndef AFUN_IT_EXCEPTION_INLINE_H
#define AFUN_IT_EXCEPTION_INLINE_H

#include "it-exception.h"

namespace aFunit {
    inline aFunitException::aFunitException(const std::string &msg) : aFunException{msg} {

    }

    inline readerFileOpenError::readerFileOpenError(const aFuntool::FilePath &file)
        : aFunitException("Reader cannot open file: " + file) {

    }
}

#endif //AFUN_IT_EXCEPTION_INLINE_H
