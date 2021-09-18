#ifndef AFUN_DLC_H
#define AFUN_DLC_H
#include "dlfcn.h"  // CMake 处理 dlfcn.h 的位置

/* 动态库工具(dlc): 处理动态库的使用 */

/*
 * NEW_DLC_SYMBOL: 用于定义指定类型的symbol结构体
 * DLC_SYMBOL: 指定类型的symbol结构体名
 * GET_SYMBOL: 访问symbol成员的值
 * MAKE_SYMBOL: 生成一个symbol
 * COPY_SYMBOL: 拷贝一个symbol(拷贝其引用)
 * READ_SYMBOL: 在dlc中获取一个symbol
 * FREE_SYMBOL： 释放symbol
 *
 * openLibary: 打开动态库
 * freeLibary: 释放动态库
 * dlcExit: 释放所有动态库
 */

#define DEFINE_DLC_SYMBOL(NAME) typedef struct DLC##NAME##SYMBOL *pDLC##NAME##SYMBOL
#define NEW_DLC_SYMBOL(TYPE, NAME) typedef struct DLC##NAME##SYMBOL { \
TYPE *symbol; \
struct DlcHandle *dlc; \
} DLC##NAME##SYMBOL, *pDLC##NAME##SYMBOL

#define DLC_SYMBOL(NAME) pDLC##NAME##SYMBOL
#define GET_SYMBOL(SYMBOL) (*((SYMBOL)->symbol))
#define MAKE_SYMBOL(symbol, TYPE) ((pDLC##TYPE##SYMBOL) (makeSymbol_(symbol)))
#define COPY_SYMBOL(ds, TYPE) ((pDLC##TYPE##SYMBOL) (copySymbol_((DlcSymbol_ *)(ds))))
#define READ_SYMBOL(dlc, name, TYPE) ((pDLC##TYPE##SYMBOL) (getSymbol_((dlc), (name))))
#define FREE_SYMBOL(symbol) ((symbol) != NULL ? (freeSymbol_((DlcSymbol_ *)(symbol)), NULL) : NULL)

typedef struct DlcSymbol_ DlcSymbol_;
typedef struct DlcHandle DlcHandle;

struct DlcHandle *openLibary(const char *file, int mode);
struct DlcSymbol_ *makeSymbol_(void *symbol);
struct DlcSymbol_ *copySymbol_(struct DlcSymbol_ *ds);
struct DlcSymbol_ *getSymbol_(struct DlcHandle *dlc, const char *name);
void freeSymbol_(struct DlcSymbol_ *symbol);
bool freeLibary(struct DlcHandle *dlc);
void dlcExit(void);


#endif //AFUN_DLC_H
