/*
 * 文件名: prefix_macro
 * 目标: 前缀的宏定义
 */
#ifndef AFUN__PREFIX_MACRO_H
#define AFUN__PREFIX_MACRO_H

// 作为顶层代码，以及'()运行时
#define L_NOT_REPEAT      (0)  /* 字面量前缀: 不重复构造 */
#define V_QUOTE           (1)  /* 变量前缀: 引用 */

#define B_EXEC            (2)  /* 括号前缀: 顺序执行 */
#define B_EXEC_FIRST      (3)  /* 括号前缀: 顺序执行, 返回第一个 */
#define B_MUST_COMMON_ARG (4)  /* 括号前缀: 强制普通参数调用 */
#define B_NOT_STRICT      (5)  /* 括号前缀: 非严格参数匹配调用 */

#define PREFIX_SIZE       (6)  /* 前缀总数 */

#endif //AFUN__PREFIX_MACRO_H
