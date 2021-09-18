#ifndef AFUN_AFUNCORE_H
#define AFUN_AFUNCORE_H
#include "tool.h"

#include "magic_func.h"
#include "prefix_macro.h"
#include "obj_api.h"

#include "parserl_warning_error.h"
#include "runtime_error.h"

#include "run.h"
#include "code.h"
#include "env.h"
#include "gc.h"
#include "func.h"
#include "object.h"
#include "var.h"
#include "parser.h"
#include "reader.h"
#include "token.h"

// Init系列函数
void aFunInit(void);

#endif //AFUN_AFUNCORE_H
