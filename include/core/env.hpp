#ifndef AFUN_ENV
#define AFUN_ENV
#include "aFunCoreExport.h"
#include "tool.hpp"
#include "prefix_macro.h"

typedef struct af_Environment af_Environment;
typedef struct af_Message af_Message;
typedef struct af_ErrorInfo af_ErrorInfo;
typedef struct af_ImportInfo af_ImportInfo;
typedef struct af_GuardianList af_GuardianList, **paf_GuardianList;

/* 顶层消息处理器的处理函数 DLC */
typedef void TopMsgProcessFunc(af_Message *msg, bool is_top, af_Environment *env);
DEFINE_DLC_SYMBOL(TopMsgProcessFunc);

typedef af_GuardianList *GuardianFunc(const char *type, bool is_guard, void *data, af_Environment *env);
DEFINE_DLC_SYMBOL(GuardianFunc);

typedef void GuardianDestruct(const char *type, void *data, af_Environment *env);
DEFINE_DLC_SYMBOL(GuardianDestruct);

enum GcRunTime {
    grt_always = 0,  // 总是运行
    grt_count,  // 累计式运行
};

#include "code.hpp"
#include "object.hpp"
#include "var.hpp"

/* 运行环境 创建与释放 */
AFUN_CORE_EXPORT af_Environment *makeEnvironment(enum GcRunTime grt);
AFUN_CORE_EXPORT bool freeEnvironment(af_Environment *env);
AFUN_CORE_EXPORT af_Environment *deriveEnvironment(bool derive_tmp, bool derive_guardian, bool derive_lr, bool enable,
                                                   af_Environment *base);

/* 消息 创建与释放 */
AFUN_CORE_EXPORT af_Message *makeMessage(const char *type, size_t size);
AFUN_CORE_EXPORT af_Message *freeMessage(af_Message *msg);
AFUN_CORE_EXPORT bool freeMessageCount(size_t count, af_Message *msg);
AFUN_CORE_EXPORT af_Message *makeNORMALMessage(af_Object *obj, af_Environment *env);
AFUN_CORE_EXPORT af_Message *makeERRORMessage(const char *type, const char *error, af_Environment *env);
AFUN_CORE_EXPORT af_Message *makeERRORMessageFormat(const char *type, af_Environment *env, const char *format, ...);
AFUN_CORE_EXPORT af_Message *makeIMPORTMessage(const char *mark, af_Object *obj, af_Environment *env);

/* ErrorInfo 创建与释放 */
AFUN_CORE_EXPORT af_ErrorInfo *makeErrorInfo(const char *type, const char *error, const char *note, FileLine line, FilePath path);
AFUN_CORE_EXPORT void freeErrorInfo(af_ErrorInfo *ei, af_Environment *env);

/* ImportInfo 创建与释放 */
af_ImportInfo *makeImportInfo(const char *mark, af_Object *obj, af_Environment *env);
void freeImportInfo(af_ImportInfo *ii, af_Environment *env);

/* 运行环境 相关操作 */
AFUN_CORE_EXPORT void enableEnvironment(af_Environment *env);
AFUN_CORE_EXPORT void setGcMax(int32_t max, af_Environment *env);
AFUN_CORE_EXPORT void setGcRun(enum GcRunTime grt, af_Environment *env);
AFUN_CORE_EXPORT char setPrefix(size_t name, char prefix, af_Environment *env);
AFUN_CORE_EXPORT void setCoreStop(af_Environment *env);
AFUN_CORE_EXPORT void setCoreExit(int exit_code, af_Environment *env);
AFUN_CORE_EXPORT void setCoreExitNotExitCode(af_Environment *env);
AFUN_CORE_EXPORT void setCoreNormal(af_Environment *env);
AFUN_CORE_EXPORT void setArgc(int argc, af_Environment *env);
AFUN_CORE_EXPORT void setEnviromentExit_out(af_Environment *env);
AFUN_CORE_EXPORT void waitForEnviromentExit(af_Environment *env);

/* 消息 相关操作 */
AFUN_CORE_EXPORT void pushMessageUp(af_Message *msg, af_Environment *env);
AFUN_CORE_EXPORT void *popMessageUpData(const char *type, af_Environment *env);
AFUN_CORE_EXPORT af_Message *popMessageUp(af_Environment *env);
AFUN_CORE_EXPORT void *getMessageData(af_Message *msg);
AFUN_CORE_EXPORT void pushMessageDown(af_Message *msg, af_Environment *env);
AFUN_CORE_EXPORT af_Message *popMessageDown(const char *type, af_Environment *env);
AFUN_CORE_EXPORT af_Message *getFirstMessage(af_Environment *env);

/* 环境变量 相关操作 */
AFUN_CORE_EXPORT void setEnvVarData(const char *name, const char *data, af_Environment *env);
AFUN_CORE_EXPORT void setEnvVarNumber(const char *name, int32_t data, af_Environment *env);

/* 顶层消息处理器 相关操作 */
AFUN_CORE_EXPORT bool addTopMsgProcess(const char *type, DLC_SYMBOL(TopMsgProcessFunc) func, af_Environment *env);

/* 顶层消息处理器 相关操作 */
AFUN_CORE_EXPORT bool addGuardian(const char *type, bool always, bool derive, size_t size, DLC_SYMBOL(GuardianFunc) func,
                                  DLC_SYMBOL(GuardianDestruct) destruct, void **pdata, af_Environment *env);
AFUN_CORE_EXPORT bool popGuardian(const char *type, af_Environment *env);

/* LiteralRegex 相关操作 */
AFUN_CORE_EXPORT bool pushLiteralRegex(const char *pattern, const char *func, bool in_protect, af_Environment *env);

/* ErrorInfo 相关操作 */
AFUN_CORE_EXPORT void fprintfErrorInfo(FILE *file, af_ErrorInfo *ei);
AFUN_CORE_EXPORT void fprintfErrorInfoStderr(af_ErrorInfo *ei);
AFUN_CORE_EXPORT void fprintfErrorInfoStdout(af_ErrorInfo *ei);

/* ErrorBacktracking 相关操作 */
AFUN_CORE_EXPORT void pushErrorBacktracking(FileLine line, FilePath file, const char *note, af_ErrorInfo *ei);

/* GuardianList 相关操作 */
AFUN_CORE_EXPORT void pushGuardianList(af_Object *obj, af_Object *func, paf_GuardianList &pal, af_Environment *env);

/* 环境变量 属性访问 */
AFUN_CORE_EXPORT char *findEnvVarData(const char *name, af_Environment *env);
AFUN_CORE_EXPORT int32_t *findEnvVarNumber(const char *name, af_Environment *env);

/* 运行环境 属性访问 */
AFUN_CORE_EXPORT char getPrefix(size_t name, af_Environment *env);
AFUN_CORE_EXPORT af_Object *getBaseObject(const char *name, af_Environment *env);
AFUN_CORE_EXPORT af_VarSpace *getProtectVarSpace(af_Environment *env);
AFUN_CORE_EXPORT int32_t getGcCount(af_Environment *env);
AFUN_CORE_EXPORT int32_t getGcMax(af_Environment *env);
AFUN_CORE_EXPORT enum GcRunTime getGcRun(af_Environment *env);
AFUN_CORE_EXPORT int getArgc(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getCoreGlobal(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getGlobal(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getBelong(af_Environment *env);
AFUN_CORE_EXPORT FilePath getActivityFile(af_Environment *env);
AFUN_CORE_EXPORT FileLine getActivityLine(af_Environment *env);
AFUN_CORE_EXPORT af_VarList *getRunVarSpaceList(af_Environment *env);
AFUN_CORE_EXPORT int isCoreExit(af_Environment *env);
AFUN_CORE_EXPORT bool getErrorStd(af_Environment *env);
AFUN_CORE_EXPORT int32_t getCoreExitCode(af_Environment *env);
AFUN_CORE_EXPORT af_Object *getActivityFunc(af_Environment *env);
AFUN_CORE_EXPORT bool isEnviromentExit(af_Environment *env);
AFUN_CORE_EXPORT size_t getEnviromentSonCount(af_Environment *env);

/* ErrorInfo 属性访问 */
AFUN_CORE_EXPORT char *getErrorType(af_ErrorInfo *ei);
AFUN_CORE_EXPORT char *getError(af_ErrorInfo *ei);

/* ImportInfo 属性访问 */
AFUN_CORE_EXPORT char *getImportMark(af_ImportInfo *ii);
AFUN_CORE_EXPORT af_Object *getImportObject(af_ImportInfo *ii, af_Environment *env);

#endif //AFUN_ENV
