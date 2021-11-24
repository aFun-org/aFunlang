#ifndef AFUN_PATH_H
#define AFUN_PATH_H
#include "base.h"

/* 路径工具 */
#ifdef aFunWIN32_NO_CYGWIN

#define SEP "\\"
#define SEP_CH '\\'

#else

#define SEP "/"
#define SEP_CH '/'

#endif

#define SHARED_PREFIX aFunSharePrefix
#define SHARED_SUFFIX aFunShareSuffix

#define STATIC_PREFIX aFunStaticPrefix
#define STATIC_SUFFIX aFunStaticSuffix

#define EXE_SUFFIX aFunExeSuffix

#endif //AFUN_PATH_H
