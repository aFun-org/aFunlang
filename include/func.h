#ifndef AFUN__FUNC_H_PUBLIC
#define AFUN__FUNC_H_PUBLIC
typedef struct af_ArgCodeList af_ArgCodeList;
typedef struct af_ArgList af_ArgList;
typedef struct af_FuncInfo af_FuncInfo;

#include "code.h"
#include "object.h"

enum af_FuncInfoScope {
    normal_scope = 0,
    inline_scope,
    pure_scope,
    super_pure_scope,
};

enum af_FuncInfoEmbedded {
    not_embedded = 0,
    protect_embedded,  // 内嵌函数
    super_embedded,  // 超内嵌函数
};

typedef void callFuncBody(void *make, af_Environment *env);
DEFINE_DLC_SYMBOL(callFuncBody);

/* af_ArgCodeList 创建与释放 */
af_ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func);
af_ArgCodeList *freeArgCodeList(af_ArgCodeList *acl);
void freeAllArgCodeList(af_ArgCodeList *acl);

/* af_ArgCodeList 操作函数 */
af_ArgCodeList **pushArgCodeList(af_ArgCodeList **base, af_ArgCodeList *new);
af_ArgCodeList **pushNewArgCodeList(af_ArgCodeList **base, af_Code *code, size_t size, bool free_code, bool run_in_func);
void *getArgCodeListData(af_ArgCodeList *acl);
af_Object *getArgCodeListResult(af_ArgCodeList *acl);

/* af_ArgList 创建与释放 */
af_ArgList *makeArgList(char *name, af_Object *obj);
af_ArgList *freeArgList(af_ArgList *al);
void freeAllArgList(af_ArgList *al);

/* af_ArgList 操作函数 */
af_ArgList **pushArgList(af_ArgList **base, af_ArgList *new);
af_ArgList **pushNewArgList(af_ArgList **base, char *name, af_Object *obj);

bool runArgList(af_ArgList *al, af_VarSpaceListNode *vsl, af_Environment *env);

/* FuncInfo 创建与释放 */
af_FuncInfo *makeFuncInfo(enum af_FuncInfoScope scope, enum af_FuncInfoEmbedded embedded, bool is_macro);

void freeFuncInfo(af_FuncInfo *fi);

/* FuncInfo 操作函数 */
void makeCFuncBodyToFuncInfo(DLC_SYMBOL(callFuncBody) c_func, char **msg_type, af_FuncInfo *fi);
void makeCodeFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi);

#endif //AFUN__FUNC_H_PUBLIC
