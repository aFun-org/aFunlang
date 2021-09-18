#ifndef AFUN_RUN_H
#define AFUN_RUN_H
#include "tool.h"
#include "code.h"

/* 代码运行工具 */
bool iterCode(af_Code *code, af_Environment *env);
bool iterDestruct(int deep, af_Environment *env);
#endif //AFUN_RUN_H
