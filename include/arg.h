#ifndef AFUN__ARG_H_PUBLIC
#define AFUN__ARG_H_PUBLIC
#include "code.h"
#include "object.h"

typedef struct ArgCodeList ArgCodeList;
typedef struct ArgList ArgList;

/* ArgCodeList 创建与释放 */
ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func);
ArgCodeList *freeArgCodeList(ArgCodeList *acl);
void freeAllArgCodeList(ArgCodeList *acl);

/* ArgCodeList 操作函数 */
ArgCodeList **pushArgCodeList(ArgCodeList **base, ArgCodeList *new);
ArgCodeList **pushNewArgCodeList(ArgCodeList **base, af_Code *code, size_t size, bool free_code, bool run_in_func);

/* ArgList 创建与释放 */
ArgList *makeArgList(char *name, af_Object *obj);
ArgList *freeArgList(ArgList *al);
void freeAllArgList(ArgList *al);

/* ArgList 操作函数 */
ArgList **pushArgList(ArgList **base, ArgList *new);
ArgList **pushNewArgList(ArgList **base, char *name, af_Object *obj);
bool runArgList(ArgList *al, af_VarSpaceListNode *vsl);

#endif //AFUN__ARG_H_PUBLIC
