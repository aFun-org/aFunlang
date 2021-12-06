#ifndef AFUN_REGEX
#define AFUN_REGEX

#define REGEX_ERROR_SIZE (512)

#include "aFunToolExport.h"

typedef struct af_Regex af_Regex;

AFUN_TOOL_EXPORT af_Regex *makeRegex(const char *pattern, char **error);
AFUN_TOOL_EXPORT void freeRegex(af_Regex *rg);
AFUN_TOOL_EXPORT int matchRegex(const char *subject, af_Regex *rg, char **error);

#endif //AFUN_REGEX
