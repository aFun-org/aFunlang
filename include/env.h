#ifndef AFUN__ENV_H_PUBLIV
#define AFUN__ENV_H_PUBLIV

typedef struct af_Environment af_Environment;

#include "bytecode.h"
#include "object.h"
#include "var.h"

af_Object *getBaseObject(char *name, af_Environment *env);

af_Environment *makeEnvironment(void);
bool enableEnvironment(af_ByteCode *bt, af_Environment *env);
void freeEnvironment(af_Environment *env);
void pushActivity(af_ByteCode *bt, bool new_vs, af_VarSpaceListNode *vsl, af_Object *belong,
                  af_Environment *env);
void popActivity(af_Environment *env);

#endif //AFUN__ENV_H_PUBLIV
