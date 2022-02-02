#ifndef AFUN_FFLAGS_H
#define AFUN_FFLAGS_H
#include "fflags.h"

ff_defArg(help, false)
        ff_argRule('h', help, not, 'h')
        ff_argRule('v', version, not, 'v')
ff_endArg(help, false);

ff_selfProcessChild(run, true);

ff_defArg(cl, false)
        ff_argRule('e', eval, must, 'e')
        ff_argRule('f', file, must, 'f')
        ff_argRule('s', source, must, 's')
        ff_argRule('b', byte, must, 'b')
        ff_argRule(aFuntool::NUL, no-aub, not, 'a')
        ff_argRule(aFuntool::NUL, no-cl, not, 'n')
        ff_argRule(aFuntool::NUL, no-import, not, 'o')
        ff_argRule(aFuntool::NUL, import, not, 'i')
ff_endArg(cl, false);

ff_defArg(build, false)
        ff_argRule('o', out, must, 'o')
        ff_argRule('p', path, must, 'p')
        ff_argRule('f', fource, not, 'f')
ff_endArg(build, false);

ff_childList(aFunlang_exe, ff_child(run), ff_child(help), ff_child(cl), ff_child(build));

#endif //AFUN_FFLAGS_H
