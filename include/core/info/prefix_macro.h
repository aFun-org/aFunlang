/*
 * 文件名: prefix_macro
 * 目标: 前缀的宏定义
 */
#ifndef AFUN_PREFIX_MACRO_H
#define AFUN_PREFIX_MACRO_H

#define E_PREFIX ",`'"  /* element前缀 */
#define B_PREFIX ",`'%^&<?>"  /* block前缀 */
#define ALL_PREFIX B_PREFIX

// 作为顶层代码，以及'()运行时
#define E_QUOTE           (0)  /* element前缀: 引用 */

#define B_EXEC            (1)  /* block前缀: 顺序执行 */
#define B_EXEC_FIRST      (2)  /* block前缀: 顺序执行, 返回第一个 */

#define PREFIX_SIZE       (3)  /* 前缀总数 */

#endif //AFUN_PREFIX_MACRO_H
