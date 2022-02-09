#ifndef AFUN_PATH_H
#define AFUN_PATH_H
#include "macro.h"
#include "tool-type.h"

/* 路径工具 */
#ifdef AFUN_WIN32_NO_CYGWIN

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
    AFUN_STATIC const char *SHARED_PREFIX = AFUN_SHARE_PREFIX;
    AFUN_STATIC const char *SHARED_SUFFIX = AFUN_SHARE_SUFFIX;

    AFUN_STATIC const char *STATIC_PREFIX = AFUN_STATIC_PREFIX;
    AFUN_STATIC const char *STATIC_SUFFIX = AFUN_STATIC_SUFFIX;

    AFUN_STATIC const char *EXE_SUFFIX = AFUN_EXE_SUFFIX;
}

#endif //AFUN_PATH_H
