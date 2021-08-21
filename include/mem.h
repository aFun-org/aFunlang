/*
 * 文件名: mem.h
 * 目标: 内存管理工具的头文件
 */

#ifndef AFUN__MEM_H
#define AFUN__MEM_H

#include <stdlib.h>
#define free(p) ((((p)!=NULL) ? (free(p), NULL) : NULL), (p)=NULL)  // free不是可选的宏

#if BUILD_MEM

/* 取代calloc函数 */
static void *safeCalloc(size_t n, size_t size);
static void *safeCalloc(size_t n, size_t size) {
    void *re = calloc(n, size);
    if (re == NULL)
        exit(1);
    return re;
}

#define calloc(n, size) (safeCalloc(n, size))

#endif
#endif  // AFUN__MEM_H