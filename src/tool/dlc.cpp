#include "tool.hpp"

#ifndef GC_SZIE
#define GC_SZIE (0)
#endif

static bool freeLibary_(struct DlcHandle *dlc);

struct DlcSymbol_ {
    void *symbol;
    struct DlcHandle *dlc;
};

struct DlcHandle {
    void *handle;
    long long int link;  // 引用计数
    long long int count;  // 开启计数
    struct DlcHandle *next;
    struct DlcHandle *last;
};

static struct DlcHandle *dlc_l = nullptr;
static int gc_count = 0;

struct DlcHandle *openLibary(const char *file, int mode) {
    void *handle = dlopen(file, mode);
    struct DlcHandle *dlc;

    if (handle == nullptr)
        return nullptr;

    for (struct DlcHandle *tmp = dlc_l; tmp != nullptr; tmp = tmp->next) {
        if (tmp->handle == handle) {
            dlclose(handle);  // 减少dlopen时对handle的引用计数
            tmp->count++;
            return tmp;
        }
    }

    dlc = calloc(1, struct DlcHandle);
    dlc->handle = handle;

    dlc->count = 1;
    dlc->next = dlc_l;
    dlc->last = nullptr;
    if (dlc_l != nullptr)
        dlc_l->last = dlc;
    dlc_l = dlc;

    return dlc;
}

bool freeLibary(struct DlcHandle *dlc) {
    if (dlc->count == 0)
        return false;
    dlc->count--;
    return true;
}

static bool freeLibary_(struct DlcHandle *dlc){
    if (dlc->link != 0)  // f - 强制释放
        return false;

    dlclose(dlc->handle);

    if (dlc->last == nullptr)
        dlc_l = dlc->next;
    else
        dlc->last->next = dlc->next;

    if (dlc->next != nullptr)
        dlc->next->last = dlc->last;

    free(dlc);
    return true;
}

static bool freeLibary_Exit(struct DlcHandle *dlc) {
    if (dlc->last == nullptr)
        dlc_l = dlc->next;
    else
        dlc->last->next = dlc->next;

    /* atexit函数中不使用dlclose */

    if (dlc->next != nullptr)
        dlc->next->last = dlc->last;

    free(dlc);
    return true;
}

static void blindSymbol(struct DlcSymbol_ *ds, struct DlcHandle *dlc) {
    if (ds->dlc != nullptr)
        ds->dlc->link--;

    ds->dlc = dlc;
    dlc->link++;
}

struct DlcSymbol_ *makeSymbol_(DlcHandle *dlc, void *symbol) {
    auto ds = calloc(1, struct DlcSymbol_);
    ds->symbol = symbol;

    if (dlc != nullptr)
        blindSymbol(ds, dlc);
    return ds;
}

struct DlcSymbol_ *copySymbol_(struct DlcSymbol_ *ds) {
    if (ds == nullptr)
        return nullptr;

    auto new_symbol = calloc(1, struct DlcSymbol_);
    new_symbol->symbol = ds->symbol;
    if (ds->dlc != nullptr)
        blindSymbol(new_symbol, ds->dlc);
    return new_symbol;
}

struct DlcSymbol_ *getSymbol_(struct DlcHandle *dlc, const char *name) {
    void *symbol = dlsym(dlc->handle, name);
    if (symbol == nullptr)
        return nullptr;

    auto ds = calloc(1, struct DlcSymbol_);
    ds->symbol = symbol;
    blindSymbol(ds, dlc);
    return ds;
}

void dlcGC() {
    for (struct DlcHandle *tmp = dlc_l, *next; tmp != nullptr; tmp = next) {
        next = tmp->next;
        if (tmp->link == 0 && tmp->count == 0)
            freeLibary_(tmp);
    }
}

void freeSymbol_(struct DlcSymbol_ *symbol) {
    if (symbol->dlc != nullptr) {
        symbol->dlc->link--;
        gc_count++;
    }

    free(symbol);

    if (gc_count >= GC_SZIE)
        dlcGC();
}

void dlcExit() {
    while (dlc_l != nullptr) {
        auto next = dlc_l->next;
        free(dlc_l);
        dlc_l = next;
    }
}
