#ifndef AFUN__CJSON_H
#define AFUN__CJSON_H
#include "macro.h"
#include "../src/deps/cjson/cJSON.h"

void cJsonInit();
cJSON *parseJsonFile(FILE *file);

#endif // AFUN__CJSON_H
