/*
 * 文件名: mem.h
 * 目标: 内存管理工具的头文件
 */

#ifndef AFUN_MEM_H
#define AFUN_MEM_H

#include <stdlib.h>
#include "log.h"
#define free(p) ((((p)!=NULL) ? (free(p), NULL) : NULL), (p)=NULL)  // free不是可选的宏

#if BUILD_MEM

/* 取代calloc函数 */
static void *safeCalloc(size_t n, size_t size);
static void *safeCalloc(size_t n, size_t size) {
    void *re = calloc(n, size);
    if (re == NULL)
        writeFatalErrorLog(NULL, EXIT_FAILURE, "The memory error");
    return re;
}

#define calloc_bak(n, size) (safeCalloc(n, size))  /* 备份 */
#define calloc calloc_bak

#endif
#endif  // AFUN_MEM_H