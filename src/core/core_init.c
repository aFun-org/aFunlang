/*
 * 文件名: core_init.c
 * 目标: 初始化函数
 */

#include "core_init.h"
#include "tool.h"
#include <locale.h>

bool aFunCoreInit(void) {
    getEndian();
    if (setlocale(LC_ALL, "") == NULL)
        return false;
#ifdef aFunWIN32
    if(!SetConsoleOutputCP(65001))  // 设置windows代码页为utf-8编码
        return false;
#endif
    printf("try 中文\n");
    return true;
}
