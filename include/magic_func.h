/*
 * 文件名: magic_func.h
 * 目标: 定义魔法函数的名字
 */
#ifndef AFUN__MAGIC_FUNC_H
#define AFUN__MAGIC_FUNC_H
/* 魔法函数 */
#define MAGIC_NAME_BASE(type) "magic-" #type ":"
#define MAGIC_NAME(type, job) (MAGIC_NAME_BASE(type) #job)

/* gc模块 */
#define mg_gc_destruct MAGIC_NAME(gc, destruct)


/* 内置环境变量 */
#define SYS_NAME(job) ("sys-" #job)

/* gc模块 */
#define ev_sys_prefix SYS_NAME(prefix)

#endif //AFUN__MAGIC_FUNC_H
