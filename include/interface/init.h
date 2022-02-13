#ifndef AFUN_INIT_H
#define AFUN_INIT_H
#include "aFunlangExport.h"
#include "aFuncore.h"
#include "aFunparser.h"
#include "aFuncode.h"
#include "aFuntool.h"

namespace aFunit {
    AFUN_LANG_EXPORT extern aFuntool::Logger *aFunLogger;
    AFUN_LANG_EXPORT bool aFunInit();
    AFUN_STATIC void setAFunLogger(aFuntool::Logger *log);
}

#include "init.inline.h"

#endif //AFUN_INIT_H
