#include "__func.h"

/* FuncBody 创建与释放 */
static af_FuncBody *makeFuncBody(enum af_FuncBodyType type, char **msg_type);

/* msg_type 释放 */
static void freeMsgType(char **msg_type);

/* FuncBody 操作函数 */
static void pushFuncBody(af_FuncBody **base, af_FuncBody *body);

af_ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func) {
    af_ArgCodeList *acl = calloc(1, sizeof(af_ArgCodeList));
    acl->info = calloc(1, size);
    acl->size = size;
    acl->code = code;
    acl->free_code = free_code;
    acl->run_in_func = run_in_func;
    return acl;
}

af_ArgCodeList *freeArgCodeList(af_ArgCodeList *acl) {
    af_ArgCodeList *next = acl->next;
    free(acl->info);
    if (acl->free_code)
        freeAllCode(acl->code);
    if (acl->result != NULL)
        gc_delReference(acl->result);
    free(acl);
    return next;
}

void freeAllArgCodeList(af_ArgCodeList *acl) {
    while (acl != NULL)
        acl = freeArgCodeList(acl);
}

af_ArgCodeList **pushArgCodeList(af_ArgCodeList **base, af_ArgCodeList *new) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = new;

    while (*base != NULL)
        base = &((*base)->next);
    return base;
}

af_ArgCodeList **pushNewArgCodeList(af_ArgCodeList **base, af_Code *code, size_t size, bool free_code, bool run_in_func) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = makeArgCodeList(code, size, free_code, run_in_func);
    return &((*base)->next);
}

void *getArgCodeListData(af_ArgCodeList *acl) {
    return acl->info;
}

af_Object *getArgCodeListResult(af_ArgCodeList *acl) {
    return acl->result;
}

af_ArgList *makeArgList(char *name, af_Object *obj) {
    af_ArgList *arg_list = calloc(1, sizeof(af_ArgList));
    arg_list->name = strCopy(name);
    arg_list->obj = obj;
    gc_addReference(obj);
    return arg_list;
}

af_ArgList *freeArgList(af_ArgList *al) {
    af_ArgList *next = al->next;
    free(al->name);
    if (al->obj != NULL)
        gc_delReference(al->obj);
    free(al);
    return next;
}

void freeAllArgList(af_ArgList *al) {
    while (al != NULL)
        al = freeArgList(al);
}

af_ArgList **pushArgList(af_ArgList **base, af_ArgList *new) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = new;

    while (*base != NULL)
        base = &((*base)->next);
    return base;
}

af_ArgList **pushNewArgList(af_ArgList **base, char *name, af_Object *obj) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = makeArgList(name, obj);
    return &((*base)->next);
}

bool runArgList(af_ArgList *al, af_VarSpaceListNode *vsl, af_Environment *env){
    for (NULL; al != NULL; al = al->next) {
        if (!makeVarToVarSpaceList(al->name, 3, 3, 3, al->obj, vsl,
                                   env->activity->belong, env))
            return false;
    }
    return true;
}

static af_FuncBody *makeFuncBody(enum af_FuncBodyType type, char **msg_type) {
    af_FuncBody *fb = calloc(1, sizeof(af_FuncBody));
    fb->type = type;
    fb->msg_type = msg_type;
    return fb;
}

af_FuncBody *makeCodeFuncBody(af_Code *code, bool free_code, char **msg_type) {
    af_FuncBody *fb = makeFuncBody(func_body_code, msg_type);
    fb->code = code;
    fb->free_code = free_code;
    return fb;
}

af_FuncBody *makeDynamicFuncBody(void) {
    af_FuncBody *fb = makeFuncBody(func_body_dynamic, NULL);
    return fb;
}

af_FuncBody *makeCFuncBody(DLC_SYMBOL(callFuncBody) c_func, char **msg_type) {
    af_FuncBody *fb = makeFuncBody(func_body_c, msg_type);
    fb->c_func = COPY_SYMBOL(c_func, callFuncBody);
    return fb;
}

static void freeMsgType(char **msg_type) {
    for (char *tmp = *msg_type; tmp != NULL; tmp++)
        free(tmp);
    free(msg_type);
}

af_FuncBody *freeFuncBody(af_FuncBody *fb) {
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

void freeAllFuncBody(af_FuncBody *fb) {
    while (fb != NULL)
        fb = freeFuncBody(fb);
}

static void pushFuncBody(af_FuncBody **base, af_FuncBody *body) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = body;
}

af_FuncInfo *makeFuncInfo(enum af_FuncInfoScope scope, enum af_FuncInfoEmbedded embedded, bool is_macro, bool var_this, bool var_func){
    af_FuncInfo *fi = calloc(1, sizeof(af_FuncInfo));
    fi->scope = scope;
    fi->embedded = embedded;
    fi->is_macro = is_macro;
    fi->var_this = var_this;
    fi->var_func = var_func;
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

void makeDynamicFuncBodyToFuncInfo(af_FuncInfo *fi) {
    pushFuncBody(&fi->body, makeDynamicFuncBody());
}

bool pushDynamicFuncBody(af_FuncBody *new, af_FuncBody *body) {
    if (body == NULL || body->next == NULL || body->next->type != func_body_dynamic) {
        freeAllFuncBody(new);
        return false;
    }

    if (new == NULL) {
        body->next = freeFuncBody(body->next);  // 不添加任何新内容, 但释放func_body_dynamic
    } else {
        af_FuncBody **next = &new;
        while ((*next) != NULL)
            next = &((*next)->next);
        *next = freeFuncBody(body->next);  // 把func_body_dynamic后的内容添加到new的末尾
        body->next = new;
    }

    return true;
}
