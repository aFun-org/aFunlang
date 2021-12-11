#ifndef AFUN_PATH_HPP
#define AFUN_PATH_HPP
#include "base.h"

/* 路径工具 */
#ifdef aFunWIN32_NO_CYGWIN

namespace aFuntool {
    static const char *SEP = "\\";
    static const char SEP_CH = '\\';
}

#else

namespace aFuntool {
    static const char *SEP = "/";
    static const char SEP_CH = '/';
}

#endif

namespace aFuntool {
    static const char *SHARED_PREFIX = aFunSharePrefix;
    static const char *SHARED_SUFFIX = aFunShareSuffix;

    static const char *STATIC_PREFIX = aFunStaticPrefix;
    static const char *STATIC_SUFFIX = aFunStaticSuffix;

    static const char *EXE_SUFFIX = aFunExeSuffix;
}

#endif //AFUN_PATH_HPP
