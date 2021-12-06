#include "__func.hpp"

/* FuncBody 创建与释放 */
static af_FuncBody *makeFuncBody(enum af_FuncBodyType type, char **msg_type);

/* msg_type 释放 */
static void freeMsgType(char **msg_type);

af_ArgCodeList *makeArgCodeList(af_Code *code, size_t size, bool free_code, bool run_in_func) {
    auto acl = calloc(1, af_ArgCodeList);
    acl->info = calloc_size(1, size);
    acl->size = size;
    acl->code = code;
    acl->free_code = free_code;
    acl->run_in_func = run_in_func;
    return acl;
}

static af_ArgCodeList *freeArgCodeList(af_ArgCodeList *acl, af_Environment *env){
    af_ArgCodeList *next = acl->next;
    free(acl->info);
    if (acl->free_code)
        freeAllCode(acl->code);
    if (acl->result != nullptr)
        gc_delObjectReference(acl->result, env);
    free(acl);
    return next;
}

void freeAllArgCodeList(af_ArgCodeList *acl, af_Environment *env){
    while (acl != nullptr)
        acl = freeArgCodeList(acl, env);
}

af_ArgCodeList **pushArgCodeList(af_ArgCodeList **base, af_ArgCodeList *new_acl) {
    while (*base != nullptr)
        base = &((*base)->next);
    *base = new_acl;

    while (*base != nullptr)
        base = &((*base)->next);
    return base;
}

void *getArgCodeListData(af_ArgCodeList *acl) {
    return acl->info;
}

af_Object *getArgCodeListResult(af_ArgCodeList *acl) {
    return acl->result;
}

af_ArgList *makeArgList(const char *name, af_Object *obj){
    auto arg_list = calloc(1, af_ArgList);
    arg_list->name = strCopy(name);
    arg_list->obj = obj;
    return arg_list;
}

static af_ArgList *freeArgList(af_ArgList *al, af_Environment *env){
    af_ArgList *next = al->next;
    free(al->name);
    if (al->obj != nullptr)
        gc_delObjectReference(al->obj, env);
    free(al);
    return next;
}

void freeAllArgList(af_ArgList *al, af_Environment *env){
    while (al != nullptr)
        al = freeArgList(al, env);
}

af_ArgList **pushArgList(af_ArgList **base, af_ArgList *new_al) {
    while (*base != nullptr)
        base = &((*base)->next);
    *base = new_al;

    while (*base != nullptr)
        base = &((*base)->next);
    return base;
}

/**
 * af_ArgCodeList 转 af_ArgList
 * @param name 参数名
 * @param acl ArgCodeList
 * @param env 运行环境
 * @return
 */
af_ArgList *makeArgListFromArgCodeList(const char *name, af_ArgCodeList *acl, af_Environment *env) {
    af_Object *obj = getArgCodeListResult(acl);
    gc_addObjectReference(obj, env);

    af_ArgList *al = makeArgList(name, obj);
    return al;
}

bool runArgList(af_ArgList *al, af_VarList *vsl, af_Environment *env){
    for(NULL; al != nullptr; al = al->next) {
        if (!makeVarToVarSpaceList(al->name, 3, 3, 3, al->obj, vsl,
                                   env->activity->belong, env))
            return false;
    }
    return true;
}

static af_FuncBody *makeFuncBody(enum af_FuncBodyType type, char **msg_type) {
    auto fb = calloc(1, af_FuncBody);
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

af_FuncBody *makeImportFuncBody(af_Code *code, bool free_code, char **msg_type) {
    af_FuncBody *fb = makeFuncBody(func_body_import, msg_type);
    fb->code = code;
    fb->free_code = free_code;
    return fb;
}

af_FuncBody *makeDynamicFuncBody() {
    af_FuncBody *fb = makeFuncBody(func_body_dynamic, nullptr);
    return fb;
}

af_FuncBody *makeCFuncBody(DLC_SYMBOL(callFuncBody) c_func, char **msg_type) {
    af_FuncBody *fb = makeFuncBody(func_body_c, msg_type);
    fb->c_func = COPY_SYMBOL(c_func, callFuncBody);
    return fb;
}

static void freeMsgType(char **msg_type) {
    for (char *tmp = *msg_type; tmp != nullptr; tmp++)
        free(tmp);
    free(msg_type);
}

af_FuncBody *freeFuncBody(af_FuncBody *fb) {
    af_FuncBody *next = fb->next;
    if ((fb->type == func_body_code || fb->type == func_body_import) && fb->free_code)
        freeAllCode(fb->code);
    else if (fb->type == func_body_c)
        FREE_SYMBOL(fb->c_func);

    if (fb->msg_type != nullptr)
        freeMsgType(fb->msg_type);
    free(fb);
    return next;
}

void freeAllFuncBody(af_FuncBody *fb) {
    while (fb != nullptr)
        fb = freeFuncBody(fb);
}

void pushFuncBody(af_FuncBody **base, af_FuncBody *body) {
    while (*base != nullptr)
        base = &((*base)->next);
    *base = body;
}

af_FuncInfo *makeFuncInfo(enum af_FuncInfoScope scope, enum af_FuncInfoEmbedded embedded, bool is_macro, bool var_this, bool var_func){
    auto fi = calloc(1, af_FuncInfo);
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

af_FuncBody *makeCFuncBodyToFuncInfo(DLC_SYMBOL(callFuncBody) c_func, char **msg_type, af_FuncInfo *fi) {
    af_FuncBody *fb = makeCFuncBody(c_func, msg_type);
    pushFuncBody(&fi->body, fb);
    return fb;
}

af_FuncBody *makeCodeFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi) {
    af_FuncBody *fb = makeCodeFuncBody(code, free_code, msg_type);
    pushFuncBody(&fi->body, fb);
    return fb;
}

af_FuncBody *makeImportFuncBodyToFuncInfo(af_Code *code, bool free_code, char **msg_type, af_FuncInfo *fi) {
    af_FuncBody *fb = makeImportFuncBody(code, free_code, msg_type);
    pushFuncBody(&fi->body, fb);
    return fb;
}

af_FuncBody *makeDynamicFuncBodyToFuncInfo(af_FuncInfo *fi) {
    af_FuncBody *fb = makeDynamicFuncBody();
    pushFuncBody(&fi->body, fb);
    return fb;
}

bool pushDynamicFuncBody(af_FuncBody *new_fub, af_FuncBody *body) {
    if (body == nullptr || body->next == nullptr || body->next->type != func_body_dynamic) {
        freeAllFuncBody(new_fub);
        return false;
    }

    if (new_fub == nullptr) {
        body->next = freeFuncBody(body->next);  // 不添加任何新内容, 但释放func_body_dynamic
    } else {
        af_FuncBody **next = &new_fub;
        while ((*next) != nullptr)
            next = &((*next)->next);
        *next = freeFuncBody(body->next);  // 把func_body_dynamic后的内容添加到new_fub的末尾
        body->next = new_fub;
    }

    return true;
}

af_ArgCodeList *getArgCodeListNext(af_ArgCodeList *acl) {
    return acl->next;
}

af_ArgList *getArgListNext(af_ArgList *al) {
    return al->next;
}

bool getArgCodeListRunInFunc(af_ArgCodeList *acl) {
    return acl->run_in_func;
}