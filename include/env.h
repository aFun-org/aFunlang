#ifndef AFUN__ENV_H_PUBLIC
#define AFUN__ENV_H_PUBLIC
#include "macro.h"
#include "tool.h"

typedef struct af_Environment af_Environment;
typedef struct af_Message af_Message;

typedef void TopMsgProcessFunc(af_Message *msg, af_Environment *env);
NEW_DLC_SYMBOL(TopMsgProcessFunc, TopMsgProcessFunc);

#include "code.h"
#include "object.h"
#include "var.h"

af_Object *getBaseObject(char *name, af_Environment *env);

af_Environment *makeEnvironment(void);
bool addTopActivity(af_Code *code, af_Environment *env);
bool enableEnvironment(af_Environment *env);
void freeEnvironment(af_Environment *env);
bool addVarToProtectVarSpace(af_Var *var, af_Environment *env);

af_Message *makeMessage(char *type, size_t size);
af_Message *freeMessage(af_Message *msg);
void freeAllMessage(af_Message *msg);
void pushMessageUp(af_Message *msg, af_Environment *env);
void pushMessageDown(af_Message *msg, af_Environment *env);
void *popMessageUpData(char *type, af_Environment *env);
af_Message *popMessageUp(af_Environment *env);
void *getMessageData(af_Message *msg);
af_Message *popMessageDown(char *type, af_Environment *env);
af_Message *getFirstMessage(af_Environment *env);
void connectMessage(af_Message **base, af_Message *msg);

void setEnvVar(char *name, char *data, af_Environment *env);
char *findEnvVar(char *name, af_Environment *env);

void addTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                      af_Environment *env);
bool changeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                         af_Environment *env);
#endif //AFUN__ENV_H_PUBLIC
