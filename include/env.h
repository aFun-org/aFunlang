#ifndef AFUN__ENV_H_PUBLIC
#define AFUN__ENV_H_PUBLIC

typedef struct af_Environment af_Environment;
typedef struct af_Message af_Message;

#include "code.h"
#include "object.h"
#include "var.h"

af_Object *getBaseObject(char *name, af_Environment *env);

af_Environment *makeEnvironment(void);
bool enableEnvironment(af_Code *bt, af_Environment *env);
void freeEnvironment(af_Environment *env);
void pushActivity(af_Code *bt, bool new_vs, af_VarSpaceListNode *vsl, af_Object *belong,
                  af_Environment *env);
void popActivity(af_Environment *env);

af_Message *makeMessage(char *type, size_t size);
af_Message *freeMessage(af_Message *msg);
void freeAllMessage(af_Message *msg);
void pushMessageUp(af_Message *msg, af_Environment *env);
void pushMessageDown(af_Message *msg, af_Environment *env);
af_Message *popMessageUp(char *type, af_Environment *env);
af_Message *popMessageDown(char *type, af_Environment *env);

void setEnvVar(char *name, char *data, af_Environment *env);
char *findEnvVar(char *name, af_Environment *env);

#endif //AFUN__ENV_H_PUBLIC
