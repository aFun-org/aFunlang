/*
 * 文件名: prefix_macro
 * 目标: 前缀的宏定义
 */
#ifndef AFUN__PREFIX_MACRO_H
#define AFUN__PREFIX_MACRO_H

// 作为顶层代码，以及'()运行时
#define V_QUOTE           (0)  /* 变量前缀: 引用 */

#define B_EXEC            (1)  /* 括号前缀: 顺序执行 */
#define B_EXEC_FIRST      (2)  /* 括号前缀: 顺序执行, 返回第一个 */

#define PREFIX_SIZE       (3)  /* 前缀总数 */

#endif //AFUN__PREFIX_MACRO_H
