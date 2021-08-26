/*
 * 文件名: magic_func.h
 * 目标: 定义魔法函数的名字
 */
#ifndef AFUN__MAGIC_FUNC_H
#define AFUN__MAGIC_FUNC_H

#define MAGIC_NAME_BASE(type) "magic-" #type ":"
#define MAGIC_NAME(type, job) (MAGIC_NAME_BASE(type) #job)

/* gc模块 */
#define gc_destruct MAGIC_NAME(gc, destruct)

#endif //AFUN__MAGIC_FUNC_H
