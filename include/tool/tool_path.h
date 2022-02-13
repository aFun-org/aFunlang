#ifndef AFUN_TOOL_PATH_H
#define AFUN_TOOL_PATH_H
#include "tool_macro.h"
#include "tool.h"

/* 路径工具 */
#ifdef AFUN_WIN32_NO_CYGWIN

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_STATIC const char *SEP = "\\";
    AFUN_STATIC const char SEP_CH = '\\';

#ifndef AFUN_TOOL_C
}
#endif

#else

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_STATIC const char *SEP = "/";
    AFUN_STATIC const char SEP_CH = '/';

#ifndef AFUN_TOOL_C
}
#endif

#endif

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    AFUN_STATIC const char *SHARED_PREFIX = AFUN_SHARE_PREFIX;
    AFUN_STATIC const char *SHARED_SUFFIX = AFUN_SHARE_SUFFIX;

    AFUN_STATIC const char *STATIC_PREFIX = AFUN_STATIC_PREFIX;
    AFUN_STATIC const char *STATIC_SUFFIX = AFUN_STATIC_SUFFIX;

    AFUN_STATIC const char *EXE_SUFFIX = AFUN_EXE_SUFFIX;

#ifndef AFUN_TOOL_C
}
#endif

#endif //AFUN_TOOL_PATH_H
