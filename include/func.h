#ifndef AFUN__FUNC_H_PUBLIC
#define AFUN__FUNC_H_PUBLIC
typedef struct ArgCodeList ArgCodeList;
typedef struct ArgList ArgList;
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

/* ArgCodeList 创建与释放 */
ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func);
ArgCodeList *freeArgCodeList(ArgCodeList *acl);
void freeAllArgCodeList(ArgCodeList *acl);

/* ArgCodeList 操作函数 */
ArgCodeList **pushArgCodeList(ArgCodeList **base, ArgCodeList *new);
ArgCodeList **pushNewArgCodeList(ArgCodeList **base, af_Code *code, size_t size, bool free_code, bool run_in_func);
void *getArgCodeListData(ArgCodeList *acl);
af_Object *getArgCodeListResult(ArgCodeList *acl);

/* ArgList 创建与释放 */
ArgList *makeArgList(char *name, af_Object *obj);
ArgList *freeArgList(ArgList *al);
void freeAllArgList(ArgList *al);

/* ArgList 操作函数 */
ArgList **pushArgList(ArgList **base, ArgList *new);
ArgList **pushNewArgList(ArgList **base, char *name, af_Object *obj);
bool runArgList(ArgList *al, af_VarSpaceListNode *vsl);

/* FuncInfo 创建与释放 */
af_FuncInfo *makeFuncInfo(enum af_FuncInfoScope scope, enum af_FuncInfoEmbedded embedded, bool is_macro, bool is_object);

void freeFuncInfo(af_FuncInfo *fi);

/* FuncInfo 操作函数 */
void makeCFuncBodyToFuncInfo(DLC_SYMBOL(callFuncBody) c_func, char **msg_type, af_FuncInfo *fi);
void makeCodeFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi);

#endif //AFUN__FUNC_H_PUBLIC
