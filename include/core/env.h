#ifndef AFUN_ENV
#define AFUN_ENV
#include "aFunCoreExport.h"
#include "tool.h"
#include "prefix_macro.h"

typedef struct af_Environment af_Environment;
typedef struct af_Message af_Message;
typedef struct af_ErrorInfo af_ErrorInfo;
typedef struct af_ImportInfo af_ImportInfo;

/* 顶层消息处理器的处理函数 DLC */
typedef void TopMsgProcessFunc(af_Message *msg, bool is_top, af_Environment *env);
DEFINE_DLC_SYMBOL(TopMsgProcessFunc);

typedef void GuardianFunc(bool is_guard, af_Environment *env);
DEFINE_DLC_SYMBOL(GuardianFunc);

enum GcRunTime {
    grt_always = 0,  // 总是运行
    grt_count,  // 累计式运行
};

#include "code.h"
#include "object.h"
#include "var.h"

/* 运行环境 创建与释放 */
AFUN_CORE_EXPORT af_Environment *makeEnvironment(enum GcRunTime grt);
AFUN_CORE_EXPORT void freeEnvironment(af_Environment *env);

/* 消息 创建与释放 */
AFUN_CORE_EXPORT af_Message *makeMessage(char *type, size_t size);
AFUN_CORE_EXPORT af_Message *freeMessage(af_Message *msg);
AFUN_CORE_EXPORT bool freeMessageCount(size_t count, af_Message *msg);
AFUN_CORE_EXPORT af_Message *makeNORMALMessage(af_Object *obj);
AFUN_CORE_EXPORT af_Message *makeERRORMessage(char *type, char *error, af_Environment *env);
AFUN_CORE_EXPORT af_Message *makeERRORMessageFormat(char *type, af_Environment *env, const char *format, ...);
AFUN_CORE_EXPORT af_Message *makeIMPORTMessage(char *mark, af_Object *obj);

/* ErrorInfo 创建与释放 */
AFUN_CORE_EXPORT af_ErrorInfo *makeErrorInfo(char *type, char *error, char *note, FileLine line, FilePath path);
AFUN_CORE_EXPORT void freeErrorInfo(af_ErrorInfo *ei);

/* ImportInfo 创建与释放 */
af_ImportInfo *makeImportInfo(char *mark, af_Object *obj);
void freeImportInfo(af_ImportInfo *ii);

/* 运行环境 相关操作 */
AFUN_CORE_EXPORT void enableEnvironment(af_Environment *env);
AFUN_CORE_EXPORT void setGcMax(int32_t max, af_Environment *env);
AFUN_CORE_EXPORT void setGcRun(enum GcRunTime grt, af_Environment *env);
AFUN_CORE_EXPORT char setPrefix(size_t name, char prefix, af_Environment *env);
AFUN_CORE_EXPORT void setCoreStop(af_Environment *env);
AFUN_CORE_EXPORT void setCoreExit(int exit_code, af_Environment *env);
AFUN_CORE_EXPORT void setCoreNormal(af_Environment *env);

/* 消息 相关操作 */
AFUN_CORE_EXPORT void pushMessageUp(af_Message *msg, af_Environment *env);
AFUN_CORE_EXPORT void *popMessageUpData(char *type, af_Environment *env);
AFUN_CORE_EXPORT af_Message *popMessageUp(af_Environment *env);
AFUN_CORE_EXPORT void *getMessageData(af_Message *msg);
AFUN_CORE_EXPORT void pushMessageDown(af_Message *msg, af_Environment *env);
AFUN_CORE_EXPORT af_Message *popMessageDown(char *type, af_Environment *env);
AFUN_CORE_EXPORT af_Message *getFirstMessage(af_Environment *env);

/* 环境变量 相关操作 */
AFUN_CORE_EXPORT void setEnvVarData(char *name, char *data, af_Environment *env);
AFUN_CORE_EXPORT void setEnvVarNumber(char *name, int32_t data, af_Environment *env);

/* 顶层消息处理器 相关操作 */
AFUN_CORE_EXPORT bool addTopMsgProcess(char *type, DLC_SYMBOL(TopMsgProcessFunc) func, af_Environment *env);

/* 顶层消息处理器 相关操作 */
AFUN_CORE_EXPORT bool addGuardian(char *type, bool always, DLC_SYMBOL(GuardianFunc) func, af_Environment *env);
AFUN_CORE_EXPORT bool popGuardian(char *type, af_Environment *env);

/* LiteralRegex 相关操作 */
AFUN_CORE_EXPORT bool pushLiteralRegex(char *pattern, char *func, bool in_protect, af_Environment *env);

/* ErrorInfo 相关操作 */
AFUN_CORE_EXPORT void fprintfErrorInfo(FILE *file, af_ErrorInfo *ei);
AFUN_CORE_EXPORT void fprintfErrorInfoStderr(af_ErrorInfo *ei);
AFUN_CORE_EXPORT void fprintfErrorInfoStdout(af_ErrorInfo *ei);

/* ErrorBacktracking 相关操作 */
AFUN_CORE_EXPORT void pushErrorBacktracking(FileLine line, FilePath file, char *note, af_ErrorInfo *ei);

/* 环境变量 属性访问 */
AFUN_CORE_EXPORT char *findEnvVarData(char *name, af_Environment *env);

/* 运行环境 属性访问 */
AFUN_CORE_EXPORT char getPrefix(size_t name, af_Environment *env);
AFUN_CORE_EXPORT af_Object *getBaseObject(char *name, af_Environment *env);
AFUN_CORE_EXPORT af_VarSpace *getProtectVarSpace(af_Environment *env);
AFUN_CORE_EXPORT int32_t getGcCount(af_Environment *env);
AFUN_CORE_EXPORT int32_t getGcMax(af_Environment *env);
AFUN_CORE_EXPORT enum GcRunTime getGcRun(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getCoreGlobal(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getGlobal(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getBelong(af_Environment *env);
AFUN_CORE_EXPORT FilePath getActivityFile(af_Environment *env);
AFUN_CORE_EXPORT FileLine getActivityLine(af_Environment *env);
AFUN_CORE_EXPORT af_VarSpaceListNode *getRunVarSpaceList(af_Environment *env);
AFUN_CORE_EXPORT int isCoreExit(af_Environment *env);
AFUN_CORE_EXPORT bool getErrorStd(af_Environment *env);

/* 消息 属性访问 */
AFUN_CORE_EXPORT af_Object *getMsgNormalData(af_Message *msg);
AFUN_CORE_EXPORT af_ErrorInfo *getMsgErrorInfo(af_Message *msg);
AFUN_CORE_EXPORT af_ImportInfo *getMsgImportInfo(af_Message *msg);

/* ErrorInfo 属性访问 */
AFUN_CORE_EXPORT char *getErrorType(af_ErrorInfo *ei);
AFUN_CORE_EXPORT char *getError(af_ErrorInfo *ei);

/* ImportInfo 属性访问 */
AFUN_CORE_EXPORT char *getImportMark(af_ImportInfo *ii);
AFUN_CORE_EXPORT af_Object *getImportObject(af_ImportInfo *ii);

#endif //AFUN_ENV
