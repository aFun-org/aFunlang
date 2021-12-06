/*
 * 文件名: mem.h
 * 目标: 内存管理工具的头文件
 */

#ifndef AFUN_MEM_HPP
#define AFUN_MEM_HPP

#include <cstdlib>
#include "log.hpp"
#define free(p) ((((p)!=nullptr) ? (free(p), nullptr) : nullptr), (p)=nullptr)  // free不是可选的宏

#if BUILD_MEM

/* 取代calloc函数 */
static void *safeCalloc(size_t n, size_t size);
static void *safeCalloc(size_t n, size_t size) {
    void *re = calloc(n, size);
    if (re == nullptr)
        writeFatalErrorLog(nullptr, EXIT_FAILURE, "The memory error");
    return re;
}

#define calloc(n, obj) (obj *)(safeCalloc(n, sizeof(obj)))
#define calloc_size(n, size) (safeCalloc(n, size))

#endif
#endif  // AFUN_MEM_HPP