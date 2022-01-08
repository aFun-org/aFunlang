/*
 * 文件名: mem.h
 * 目标: 内存管理工具的头文件
 */

#ifndef AFUN_MEM_H
#define AFUN_MEM_H

#include "mem.inline.h"

#ifndef MEM_NOT_DEFINE
#define free(p) (aFuntool::safeFree((p)))
#define calloc(n, obj) (obj *)(aFuntool::safeCalloc(n, sizeof(obj)))
#define calloc_size(n, size) (aFuntool::safeCalloc(n, size))
#endif

#endif  // AFUN_MEM_H