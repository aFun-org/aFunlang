#ifndef AFUN__CJSON_H
#define AFUN__CJSON_H
#include "macro.h"
#include "cJSON/cJSON.h"
void cJsonInit();
cJSON *parseJsonFile(FILE *file);

#endif // AFUN__CJSON_H
