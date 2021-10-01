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
    if (re == NULL) {
        writeFatalErrorLog(NULL, log_default, EXIT_FAILURE, "The calloc error.");
    }
    return re;
}

#define calloc(n, size) (safeCalloc(n, size))

#endif
#endif  // AFUN_MEM_H