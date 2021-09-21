#ifndef AFUN_REGEX
#define AFUN_REGEX

#define REGEX_ERROR_SIZE (512)

#include "aFunToolExport.h"

typedef struct af_Regex af_Regex;
extern char regex_error[REGEX_ERROR_SIZE];

AFUN_TOOL_EXPORT af_Regex *makeRegex(char *pattern);
AFUN_TOOL_EXPORT void freeRegex(af_Regex *rg);
AFUN_TOOL_EXPORT int matchRegex(char *subject, af_Regex *rg);

#endif //AFUN_REGEX
