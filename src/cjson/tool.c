#include <stdio.h>
#include "mem.h"
#include "tool.h"
#include "cJSON/cJSON.h"

#ifndef FSTRING_SIZE
#define FSTRING_SIZE (100)
#endif

static void *safeMalloc(size_t size) {
    return calloc(1, size);
}

void cJsonInit() {
    static cJSON_Hooks hooks = {.malloc_fn=safeMalloc, .free_fn=free};
    cJSON_InitHooks(&hooks);
}

cJSON *parseJsonFile(FILE *file) {
    char *tmp = NULL;
    char buffer[FSTRING_SIZE + 1];
    cJSON *re;

    while (1) {
        fgets(buffer, FSTRING_SIZE + 1, file);
        tmp = strJoin(tmp, buffer, true, false);
        if (ferror(file) || feof(file))
            break;
    }

    re = cJSON_Parse(tmp);
    free(tmp);
    return re;
}