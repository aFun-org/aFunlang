/*
 * 文件名: magic_func.h
 * 目标: 定义魔法函数的名字
 */
#ifndef AFUN_MAGIC_FUNC_H
#define AFUN_MAGIC_FUNC_H
/* 魔法函数 */
#define MAGIC_NAME_BASE(type) "magic-" #type ":"
#define MAGIC_NAME(type, job) (MAGIC_NAME_BASE(type) #job)

/* gc模块 */
#define mg_gc_destruct MAGIC_NAME(gc, destruct)


/* 内置环境变量 */
#define SYS_NAME(job) "sys-" #job

#define ev_sys_prefix SYS_NAME(prefix)
#define ev_grt SYS_NAME(grt)
#define ev_gcmax SYS_NAME(gc-max)
#define ev_gccount SYS_NAME(gc-count)
#define ev_exit_code SYS_NAME(exit-code)
#define ev_argc SYS_NAME(argc)
#define ev_argvx_prefix SYS_NAME(argv)

#endif //AFUN_MAGIC_FUNC_H
