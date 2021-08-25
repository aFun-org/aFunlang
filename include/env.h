#ifndef AFUN__ENV_H_PUBLIC
#define AFUN__ENV_H_PUBLIC
#include "macro.h"
#include "tool.h"
#include "prefix_macro.h"

typedef struct af_Environment af_Environment;
typedef struct af_Message af_Message;

/* 顶层消息处理器的处理函数 DLC */
typedef void TopMsgProcessFunc(af_Message *msg, af_Environment *env);
DEFINE_DLC_SYMBOL(TopMsgProcessFunc);

enum GcRunTime {
    grt_always = 0,  // 总是运行
    grt_count,  // 累计式运行
};

#include "code.h"
#include "object.h"
#include "var.h"

/* 前缀管理函数 */
char getPrefix(size_t name, af_Environment *env);
char setPrefix(size_t name, char prefix, af_Environment *env);

/* 运行环境函数 */
af_Environment *makeEnvironment(enum GcRunTime grt);
bool enableEnvironment(af_Environment *env);
void freeEnvironment(af_Environment *env);
void checkRunGC(af_Environment *env);

/* 保护空间管理函数 */
bool addVarToProtectVarSpace(af_Var *var, af_Environment *env);
af_Object *getBaseObject(char *name, af_Environment *env);
af_VarSpace *getProtectVarSpace(af_Environment *env);

/* 消息创建与释放函数 */
af_Message *makeMessage(char *type, size_t size);
af_Message *freeMessage(af_Message *msg);
void freeAllMessage(af_Message *msg);
bool freeMessageCount(size_t count, af_Message *msg);

/* 消息管理函数 */
void pushMessageUp(af_Message *msg, af_Environment *env);
void *popMessageUpData(char *type, af_Environment *env);
af_Message *popMessageUp(af_Environment *env);
void *getMessageData(af_Message *msg);
void connectMessage(af_Message **base, af_Message *msg);

/* 消息工具函数 */
af_Message *makeNORMALMessage(af_Object *obj);

/* 下行消息表管理函数 */
void pushMessageDown(af_Message *msg, af_Environment *env);
af_Message *popMessageDown(char *type, af_Environment *env);
af_Message *getFirstMessage(af_Environment *env);

/* 环境变量管理函数 */
void setEnvVar(char *name, char *data, af_Environment *env);
char *findEnvVar(char *name, af_Environment *env);

/* 顶层消息处理器管理函数 */
void addTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                      af_Environment *env);
bool changeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func,
                         af_Environment *env);
#endif //AFUN__ENV_H_PUBLIC
