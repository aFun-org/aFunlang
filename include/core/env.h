#ifndef AFUN_ENV
#define AFUN_ENV
#include "aFunCoreExport.h"
#include "tool.h"
#include "prefix_macro.h"

typedef struct af_Environment af_Environment;
typedef struct af_Message af_Message;
typedef struct af_ErrorInfo af_ErrorInfo;

/* 顶层消息处理器的处理函数 DLC */
typedef void TopMsgProcessFunc(af_Message *msg, bool is_gc, af_Environment *env);
DEFINE_DLC_SYMBOL(TopMsgProcessFunc);

enum GcRunTime {
    grt_always = 0,  // 总是运行
    grt_count,  // 累计式运行
};

#include "code.h"
#include "object.h"
#include "var.h"

/* 前缀管理函数 */
AFUN_CORE_EXPORT char getPrefix(size_t name, af_Environment *env);
AFUN_CORE_EXPORT char setPrefix(size_t name, char prefix, af_Environment *env);

/* 运行环境函数 */
AFUN_CORE_EXPORT af_Environment *makeEnvironment(enum GcRunTime grt);
AFUN_CORE_EXPORT void enableEnvironment(af_Environment *env);
AFUN_CORE_EXPORT void freeEnvironment(af_Environment *env);

/* 保护空间管理函数 */
AFUN_CORE_EXPORT bool addVarToProtectVarSpace(af_Var *var, af_Environment *env);
AFUN_CORE_EXPORT af_Object *getBaseObject(char *name, af_Environment *env);
AFUN_CORE_EXPORT af_VarSpace *getProtectVarSpace(af_Environment *env);

/* Core 退出与停止 */
AFUN_CORE_EXPORT void setCoreStop(af_Environment *env);
AFUN_CORE_EXPORT void setCoreExit(int exit_code, af_Environment *env);
AFUN_CORE_EXPORT void setCoreNormal(af_Environment *env);

/* 消息创建与释放函数 */
AFUN_CORE_EXPORT af_Message *makeMessage(char *type, size_t size);
AFUN_CORE_EXPORT af_Message *freeMessage(af_Message *msg);
AFUN_CORE_EXPORT void freeAllMessage(af_Message *msg);
AFUN_CORE_EXPORT bool freeMessageCount(size_t count, af_Message *msg);

/* 消息管理函数 */
AFUN_CORE_EXPORT void pushMessageUp(af_Message *msg, af_Environment *env);
AFUN_CORE_EXPORT void *popMessageUpData(char *type, af_Environment *env);
AFUN_CORE_EXPORT af_Message *popMessageUp(af_Environment *env);
AFUN_CORE_EXPORT void *getMessageData(af_Message *msg);
AFUN_CORE_EXPORT void connectMessage(af_Message **base, af_Message *msg);

/* 消息工具函数 */
AFUN_CORE_EXPORT af_Message *makeNORMALMessage(af_Object *obj);
AFUN_CORE_EXPORT af_Message *makeERRORMessage(char *type, char *error, af_Environment *env);
AFUN_CORE_EXPORT af_Message *makeERRORMessageFormate(char *type, af_Environment *env, const char *formate, ...);

/* 下行消息表管理函数 */
AFUN_CORE_EXPORT void pushMessageDown(af_Message *msg, af_Environment *env);
AFUN_CORE_EXPORT af_Message *popMessageDown(char *type, af_Environment *env);
AFUN_CORE_EXPORT af_Message *getFirstMessage(af_Environment *env);

/* 环境变量管理函数 */
AFUN_CORE_EXPORT void setEnvVar(char *name, char *data, af_Environment *env);
AFUN_CORE_EXPORT char *findEnvVar(char *name, af_Environment *env);

/* 顶层消息处理器管理函数 */
AFUN_CORE_EXPORT void addTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func, af_Environment *env);
AFUN_CORE_EXPORT bool changeTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func, af_Environment *env);

/* LiteralRegex操作函数 */
AFUN_CORE_EXPORT bool pushLiteralRegex(char *pattern, char *func, bool in_protect, af_Environment *env);

/* ErrorInfo 创建与释放 */
AFUN_CORE_EXPORT af_ErrorInfo *makeErrorInfo(char *type, char *error, char *note, FileLine line, FilePath path);
AFUN_CORE_EXPORT void freeErrorInfo(af_ErrorInfo *ei);

/* ErrorInfo 操作函数 */
AFUN_CORE_EXPORT void fprintfErrorInfo(FILE *file, af_ErrorInfo *ei);

/* ErrorBacktracking 操作函数 */
AFUN_CORE_EXPORT void pushErrorBacktracking(FileLine line, FilePath file, char *note, af_ErrorInfo *ei);

#endif //AFUN_ENV
