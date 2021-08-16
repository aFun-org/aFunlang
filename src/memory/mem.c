/*
 * 文件名: mem.c
 * 目标: 内存管理
 * 对应头文件: mem.h
 * 由宏 BUILD_VTMEM 和宏 DEBUG_VTMEM 联合控制行为
 * BUILD_VTMEM 开启时启用该组件, 关闭时safeCalloc等函数与原生的calloc相同
 * DEBUG_VTMEM 开启时会自动统计申请成功的内存总量和释放的内存总量。
 */

#include <stdlib.h>
#include <string.h>
#include "mem.h"

#if BUILD_VTMEM
#if DEBUG_VTMEM
#include <stdio.h>
static unsigned long long used_mem = 0;  // 已经使用的内存

struct afmem_block {
    size_t size;
};

void safeFreeDebug(void *p) {
    if (p == NULL)
        return;
    p -= sizeof(struct afmem_block);  // 往回偏移, 得到malloc函数分配的真实指针
    used_mem -= ((struct afmem_block *)p)->size;
    free(p);  // 偏移
}

void *safeCalloc(size_t n, size_t size) {
    size_t r_size = n * size + sizeof(struct afmem_block);
    void *re = malloc(r_size);
    if (re == NULL)
        exit(1);
    used_mem += n * size;
    memset(re, 0, r_size);  // 清空为0, 用malloc模拟calloc的行为
    ((struct afmem_block *)re)->size = n * size;  // 记录大小
    return (char *)re + sizeof(struct afmem_block);  // 偏移
}

int print_memInfo() {
    return printf("used_mem = %llu\n", used_mem);
}

#else
// 无 void safeFree_(void *p);
// 无 print_memInfo();

void *safeCalloc(size_t n, size_t size) {
    void *re = calloc(n, size);
    if (re == NULL)
        exit(1);
    return re;
}

void safeFree_(void *p) {
    if (p != NULL)
        free(p);
}

#endif
#endif