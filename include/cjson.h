#ifndef CJSON__H
#define CJSON__H

#include "cJSON/cJSON.h"
void cJsonInit();
cJSON *parseJsonFile(FILE *file);

#endif // CJSON__H
