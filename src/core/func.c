#include "__func.h"

/* FuncBody 创建与释放 */
static af_FuncBody *makeFuncBody(enum af_FuncBodyType type, char **msg_type);
static af_FuncBody *makeCodeFuncBody(af_Code *code, bool free_code, char **msg_type);
static af_FuncBody *makeCFuncBody(DLC_SYMBOL(callFuncBody) c_func, char **msg_type);
static af_FuncBody *freeFuncBody(af_FuncBody *fb);
static void freeAllFuncBody(af_FuncBody *fb);

/* msg_type 释放 */
static void freeMsgType(char **msg_type);

/* FuncBody 操作函数 */
static void pushFuncBody(af_FuncBody **base, af_FuncBody *body);

ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func) {
    ArgCodeList *acl = calloc(sizeof(ArgCodeList), 1);
    acl->info = calloc(size, 1);
    acl->size = size;
    acl->code = code;
    acl->free_code = free_code;
    acl->run_in_func = run_in_func;
    return acl;
}

ArgCodeList *freeArgCodeList(ArgCodeList *acl) {
    ArgCodeList *next = acl->next;
    free(acl->info);
    if (acl->free_code)
        freeAllCode(acl->code);
    if (acl->result != NULL)
        gc_delReference(acl->result);
    free(acl);
    return next;
}

void freeAllArgCodeList(ArgCodeList *acl) {
    while (acl != NULL)
        acl = freeArgCodeList(acl);
}

ArgCodeList **pushArgCodeList(ArgCodeList **base, ArgCodeList *new) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = new;

    while (*base != NULL)
        base = &((*base)->next);
    return base;
}

ArgCodeList **pushNewArgCodeList(ArgCodeList **base, af_Code *code, size_t size, bool free_code, bool run_in_func) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = makeArgCodeList(code, size, free_code, run_in_func);
    return &((*base)->next);
}

ArgList *makeArgList(char *name, af_Object *obj) {
    ArgList *arg_list = calloc(sizeof(ArgList), 1);
    arg_list->name = strCopy(name);
    arg_list->obj = obj;
    gc_addReference(obj);
    return arg_list;
}

ArgList *freeArgList(ArgList *al) {
    ArgList *next = al->next;
    free(al->name);
    if (al->obj != NULL)
        gc_addReference(al->obj);
    free(al);
    return next;
}

void freeAllArgList(ArgList *al) {
    while (al != NULL)
        al = freeArgList(al);
}

ArgList **pushArgList(ArgList **base, ArgList *new) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = new;

    while (*base != NULL)
        base = &((*base)->next);
    return base;
}

ArgList **pushNewArgList(ArgList **base, char *name, af_Object *obj) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = makeArgList(name, obj);
    return &((*base)->next);
}

bool runArgList(ArgList *al, af_VarSpaceListNode *vsl) {
    for (NULL; al != NULL; al = al->next) {
        if (!makeVarToVarSpaceList(al->name, 3, 3, al->obj, vsl))
            return false;
    }
    return true;
}

static af_FuncBody *makeFuncBody(enum af_FuncBodyType type, char **msg_type) {
    af_FuncBody *fb = calloc(sizeof(af_FuncBody), 1);
    fb->type = type;
    fb->msg_type = msg_type;
    return fb;
}

static af_FuncBody *makeCodeFuncBody(af_Code *code, bool free_code, char **msg_type) {
    af_FuncBody *fb = makeFuncBody(func_body_code, msg_type);
    fb->code = code;
    fb->free_code = free_code;
    return fb;
}

static af_FuncBody *makeCFuncBody(DLC_SYMBOL(callFuncBody) c_func, char **msg_type) {
    af_FuncBody *fb = makeFuncBody(func_body_c, msg_type);
    fb->c_func = COPY_SYMBOL(c_func, callFuncBody);
    return fb;
}

static void freeMsgType(char **msg_type) {
    for (char *tmp = *msg_type; tmp != NULL; tmp++)
        free(tmp);
    free(msg_type);
}

static af_FuncBody *freeFuncBody(af_FuncBody *fb) {
    af_FuncBody *next = fb->next;
    if (fb->type == func_body_code && fb->free_code)
        freeAllCode(fb->code);
    else if (fb->type == func_body_c)
        FREE_SYMBOL(fb->c_func);

    if (fb->msg_type != NULL)
        freeMsgType(fb->msg_type);
    free(fb);
    return next;
}

static void freeAllFuncBody(af_FuncBody *fb) {
    while (fb != NULL)
        fb = freeFuncBody(fb);
}

static void pushFuncBody(af_FuncBody **base, af_FuncBody *body) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = body;
}

af_FuncInfo *makeFuncInfo(enum af_FuncInfoScope scope, enum af_FuncInfoEmbedded embedded, bool is_macro, bool is_object) {
    af_FuncInfo *fi = calloc(sizeof(af_FuncInfo), 1);
    fi->scope = scope;
    fi->embedded = embedded;
    fi->is_macro = is_macro;
    fi->is_object = is_object;
    return fi;
}

void freeFuncInfo(af_FuncInfo *fi) {  // vsl是不释放的
    freeAllFuncBody(fi->body);
    free(fi);
}

void makeCFuncBodyToFuncInfo(DLC_SYMBOL(callFuncBody) c_func, char **msg_type, af_FuncInfo *fi) {
    pushFuncBody(&fi->body, makeCFuncBody(c_func, msg_type));
}

void makeCodeFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi) {
    pushFuncBody(&fi->body, makeCodeFuncBody(code, free_code, msg_type));
}
