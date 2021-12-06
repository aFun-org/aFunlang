#ifndef AFUN_DLC_HPP
#define AFUN_DLC_HPP
#include "aFunToolExport.h"
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
#define NEW_DLC_SYMBOL(TYPE, NAME) typedef struct DLC##NAME##SYMBOL { TYPE *symbol; struct DlcHandle *dlc; } DLC##NAME##SYMBOL, *pDLC##NAME##SYMBOL

typedef struct DlcSymbol_ DlcSymbol_;
typedef struct DlcHandle DlcHandle;

#define DLC_SYMBOL(NAME) pDLC##NAME##SYMBOL
#define GET_SYMBOL(SYMBOL) (*((SYMBOL)->symbol))
#define MAKE_SYMBOL(symbol, TYPE) ((pDLC##TYPE##SYMBOL) (makeSymbol_(nullptr, (DlcSymbol_ *)(symbol))))
#define MAKE_SYMBOL_FROM_HANDLE(symbol, handle, TYPE) ((pDLC##TYPE##SYMBOL) (makeSymbol_((handle), (DlcSymbol_ *)(symbol))))
#define COPY_SYMBOL(ds, TYPE) ((pDLC##TYPE##SYMBOL) (copySymbol_((DlcSymbol_ *)(ds))))
#define READ_SYMBOL(dlc, name, TYPE) ((pDLC##TYPE##SYMBOL) (getSymbol_((dlc), (name))))
#define FREE_SYMBOL(symbol) ((symbol) != nullptr ? (freeSymbol_((DlcSymbol_ *)(symbol)), nullptr) : nullptr)

AFUN_TOOL_EXPORT DlcHandle *openLibary(const char *file, int mode);
AFUN_TOOL_EXPORT DlcSymbol_ *makeSymbol_(DlcHandle *dlc, void *symbol);
AFUN_TOOL_EXPORT DlcSymbol_ *copySymbol_(DlcSymbol_ *ds);
AFUN_TOOL_EXPORT DlcSymbol_ *getSymbol_(DlcHandle *dlc, const char *name);

AFUN_TOOL_EXPORT void freeSymbol_(DlcSymbol_ *symbol);
AFUN_TOOL_EXPORT bool freeLibary(DlcHandle *dlc);
AFUN_TOOL_EXPORT void dlcExit();


#endif //AFUN_DLC_HPP
