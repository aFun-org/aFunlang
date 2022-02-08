#ifndef AFUN_PATH_H
#define AFUN_PATH_H
#include "macro.h"
#include "tool-type.h"

/* 路径工具 */
#ifdef aFunWIN32_NO_CYGWIN

namespace aFuntool {
    AFUN_STATIC const char *SEP = "\\";
    AFUN_STATIC const char SEP_CH = '\\';
}

#else

namespace aFuntool {
    AFUN_STATIC const char *SEP = "/";
    AFUN_STATIC const char SEP_CH = '/';
}

#endif

namespace aFuntool {
    AFUN_STATIC const char *SHARED_PREFIX = aFunSharePrefix;
    AFUN_STATIC const char *SHARED_SUFFIX = aFunShareSuffix;

    AFUN_STATIC const char *STATIC_PREFIX = aFunStaticPrefix;
    AFUN_STATIC const char *STATIC_SUFFIX = aFunStaticSuffix;

    AFUN_STATIC const char *EXE_SUFFIX = aFunExeSuffix;
}

#endif //AFUN_PATH_H
