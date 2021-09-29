#ifndef AFUN_BYTECODE
#define AFUN_BYTECODE
#include <stdio.h>
#include "aFunCoreExport.h"

#define CODE_STR_MAX_SIZE (50)
typedef struct af_Code af_Code;

typedef uint32_t CodeUInt;  // Code int
typedef int64_t LayerInt;  // 只有当layer会小于0时使用

enum af_CodeType {
    code_element = 'e',
    code_block = 'm',  // 括号
};

/* 括号类型 */
enum af_BlockType {
    parentheses = '(',  // 小括号
    brackets = '[',  // 中括号
    curly = '{',  // 大括号
};

/* 代码块 创建与释放 */
AFUN_CORE_EXPORT af_Code *makeElementCode(char *var, char prefix, FileLine line, FilePath path);
AFUN_CORE_EXPORT af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix,
                                        FileLine line, FilePath path, af_Code **next);
AFUN_CORE_EXPORT void freeAllCode(af_Code *bt);

/* 代码块 相关操作 */
AFUN_CORE_EXPORT af_Code *pushCode(af_Code **base, af_Code *next);
AFUN_CORE_EXPORT af_Code *copyAllCode(af_Code *base, FilePath *path);
AFUN_CORE_EXPORT af_Code *copyCode(af_Code *base, FilePath *path);
AFUN_CORE_EXPORT bool writeAllCode(af_Code *bt, FILE *file);
AFUN_CORE_EXPORT bool readAllCode(af_Code **bt, FilePath path, FILE *file);

/* 代码块 属性访问 */
AFUN_CORE_EXPORT af_Code *getCodeNext(af_Code *bt);
AFUN_CORE_EXPORT af_Code *getCodeElement(af_Code *bt);
AFUN_CORE_EXPORT char *codeToStr(af_Code *code, int n);
AFUN_CORE_EXPORT enum af_CodeType getCodeType(af_Code *code);
AFUN_CORE_EXPORT enum af_BlockType getCodeBlockType(af_Code *code);
AFUN_CORE_EXPORT char getCodePrefix(af_Code *code);
AFUN_CORE_EXPORT CodeUInt getCodeEndCount(af_Code *code);
AFUN_CORE_EXPORT char *getCodeElementData(af_Code *code);
AFUN_CORE_EXPORT CodeUInt getCodeElementCount(af_Code *code);
AFUN_CORE_EXPORT char *getCodeMD5(af_Code *code);
AFUN_CORE_EXPORT bool codeSemanticCheck(af_Code *code);
#endif //AFUN_BYTECODE
