#ifndef AFUN_FUNC
#define AFUN_FUNC
typedef struct af_ArgCodeList af_ArgCodeList;
typedef struct af_ArgList af_ArgList;
typedef struct af_FuncInfo af_FuncInfo;
typedef struct af_FuncBody af_FuncBody;

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

#include "aFunCoreExport.h"
#include "code.h"
#include "object.h"

typedef struct af_CallFuncInfo af_CallFuncInfo;
struct af_CallFuncInfo {
    void *mark;
    af_FuncBody *body_next;

    af_Object *belong;
    af_Object *func;
    struct af_VarSpaceListNode *var_list;

    enum af_BlockType call_type;
    bool is_gc_call;
    bool is_literal;
    bool is_obj_func;
    bool is_macro_call;
};

typedef struct af_FuncBody *callFuncBody(af_CallFuncInfo *info, af_Environment *env);
NEW_DLC_SYMBOL(callFuncBody, callFuncBody);

/* af_ArgCodeList 创建与释放 */
AFUN_CORE_EXPORT af_ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func);
AFUN_CORE_EXPORT void freeAllArgCodeList(af_ArgCodeList *acl, af_Environment *env);

/* af_ArgList 创建与释放 */
AFUN_CORE_EXPORT af_ArgList *makeArgList(char *name, af_Object *obj, af_Environment *env);
AFUN_CORE_EXPORT void freeAllArgList(af_ArgList *al, af_Environment *env);

/* FuncBody 创建与释放 */
AFUN_CORE_EXPORT af_FuncBody *makeCodeFuncBody(af_Code *code, bool free_code, char **msg_type);
AFUN_CORE_EXPORT af_FuncBody *makeImportFuncBody(af_Code *code, bool free_code, char **msg_type);
AFUN_CORE_EXPORT af_FuncBody *makeCFuncBody(DLC_SYMBOL(callFuncBody) c_func, char **msg_type);
AFUN_CORE_EXPORT af_FuncBody *makeDynamicFuncBody(void);
AFUN_CORE_EXPORT af_FuncBody *freeFuncBody(af_FuncBody *fb);
AFUN_CORE_EXPORT void freeAllFuncBody(af_FuncBody *fb);

/* FuncInfo 创建与释放 */
AFUN_CORE_EXPORT af_FuncInfo *makeFuncInfo(enum af_FuncInfoScope scope, enum af_FuncInfoEmbedded embedded,
                                           bool is_macro, bool var_this, bool var_func);
AFUN_CORE_EXPORT void freeFuncInfo(af_FuncInfo *fi);

/* af_ArgCodeList 相关操作 */
AFUN_CORE_EXPORT af_ArgCodeList **pushArgCodeList(af_ArgCodeList **base, af_ArgCodeList *new);

/* af_ArgList 相关操作 */
AFUN_CORE_EXPORT af_ArgList **pushArgList(af_ArgList **base, af_ArgList *new);

/* af_FuncBody 相关操作 */
AFUN_CORE_EXPORT void pushFuncBody(af_FuncBody **base, af_FuncBody *body);

/* FuncInfo 相关操作 */
AFUN_CORE_EXPORT void makeCFuncBodyToFuncInfo(DLC_SYMBOL(callFuncBody) c_func, char **msg_type, af_FuncInfo *fi);
AFUN_CORE_EXPORT void makeCodeFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi);
AFUN_CORE_EXPORT void makeImportFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi);
AFUN_CORE_EXPORT void makeDynamicFuncBodyToFuncInfo(af_FuncInfo *fi);

/* af_ArgCodeList 属性获取 */
AFUN_CORE_EXPORT void *getArgCodeListData(af_ArgCodeList *acl);
AFUN_CORE_EXPORT af_Object *getArgCodeListResult(af_ArgCodeList *acl);
af_ArgCodeList *getArgCodeListNext(af_ArgCodeList *acl);
bool getArgCodeListRunInFunc(af_ArgCodeList *acl);

/* af_ArgList 属性获取 */
af_ArgList *getArgListNext(af_ArgList *al);

#endif //AFUN_FUNC
