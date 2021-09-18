#ifndef AFUN_REGEX
#define AFUN_REGEX

#define REGEX_ERROR_SIZE (512)

typedef struct af_Regex af_Regex;
extern char regex_error[REGEX_ERROR_SIZE];

af_Regex *makeRegex(char *pattern);
void freeRegex(af_Regex *rg);
int matchRegex(char *subject, af_Regex *rg);

#endif //AFUN_REGEX
