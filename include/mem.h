/*
 * 文件名: mem.h
 * 目标: 内存管理工具的头文件
 */

#ifndef MEM__H
#define MEM__H

#if BUILD_MEM
#include <stdlib.h>

static void *safeCalloc(size_t n, size_t size);
static void *safeCalloc(size_t n, size_t size) {
    void *re = calloc(n, size);
    if (re == NULL)
        exit(1);
    return re;
}

#define calloc(n, size) (safeCalloc(n, size))
#define free(p) ((((p)!=NULL) ? (free(p), NULL) : NULL), (p)=NULL)

#endif
#endif  // MEM__H