#include "__arg.h"

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
        if (!makeVarToVarSpaceList(al->name, 3, 3, 3, al->obj, vsl))
            return false;
    }
    return true;
}

