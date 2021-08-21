#include "mem.h"
#include "tool.h"

#ifndef GC_SZIE
#define GC_SZIE (0)
#endif

static bool freeLibary_(struct DlcHandle *dlc, bool f);

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

static struct DlcHandle *dlc_l = NULL;
static int gc_count = 0;

struct DlcHandle *openLibary(const char *file, int mode) {
    void *handle = dlopen(file, mode);
    struct DlcHandle *dlc;

    if (handle == NULL)
        return NULL;

    for (struct DlcHandle *tmp = dlc_l; tmp != NULL; tmp = tmp->next) {
        if (tmp->handle == handle) {
            dlclose(handle);  // 减少dlopen时对handle的引用计数
            tmp->count++;
            return tmp;
        }
    }

    dlc = calloc(1, sizeof(struct DlcHandle));
    dlc->handle = handle;

    dlc->count = 1;
    dlc->next = dlc_l;
    dlc->last = NULL;
    if (dlc_l != NULL)
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

static bool freeLibary_(struct DlcHandle *dlc, bool f) {
    if (dlc->link != 0 && !f)  // f - 强制释放
        return false;

    dlclose(dlc->handle);

    if (dlc->last == NULL)
        dlc_l = dlc->next;
    else
        dlc->last->next = dlc->next;

    if (dlc->next != NULL)
        dlc->next->last = dlc->last;

    free(dlc);
    return true;
}

struct DlcSymbol_ *makeSymbol_(void *symbol) {
    struct DlcSymbol_ *ds = calloc(1, sizeof(struct DlcSymbol_));
    ds->symbol = symbol;
    ds->dlc = NULL;
    return ds;
}

static void blindSymbol(struct DlcSymbol_ *ds, struct DlcHandle *dlc) {
    if (ds->dlc != NULL)
        ds->dlc->link--;

    ds->dlc = dlc;
    dlc->link++;
}

struct DlcSymbol_ *copySymbol_(struct DlcSymbol_ *ds) {
    if (ds == NULL)
        return NULL;

    struct DlcSymbol_ *new = calloc(1, sizeof(struct DlcSymbol_));
    new->symbol = ds->symbol;
    if (ds->dlc != NULL)
        blindSymbol(new, ds->dlc);
    return new;
}

struct DlcSymbol_ *getSymbol_(struct DlcHandle *dlc, const char *name) {
    void *symbol = dlsym(dlc->handle, name);
    if (symbol == NULL)
        return NULL;

    struct DlcSymbol_ *ds = calloc(1, sizeof(struct DlcSymbol_));
    ds->symbol = symbol;
    blindSymbol(ds, dlc);
    return ds;
}

void dlcGC(void) {
    for (struct DlcHandle *tmp = dlc_l, *next; tmp != NULL; tmp = next) {
        next = tmp->next;
        if (tmp->link == 0 && tmp->count == 0)
            freeLibary_(tmp, false);
    }
}

void freeSymbol_(struct DlcSymbol_ *symbol) {
    if (symbol->dlc != NULL) {
        symbol->dlc->link--;
        gc_count++;
    }

    free(symbol);

    if (gc_count >= GC_SZIE)
        dlcGC();
}

void dlcExit(void) {
    while (dlc_l != NULL)
        freeLibary_(dlc_l, true);
}
