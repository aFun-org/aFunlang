#ifndef AFUN_BYTECODE
#define AFUN_BYTECODE
#include <stdio.h>
#include "aFunCoreExport.h"

#define CODE_STR_MAX_SIZE (50)
typedef struct af_Code af_Code;

typedef long CodeInt;  // Code uint

enum af_CodeType {
    code_element = 0,
    code_block,  // 括号
};

/* 括号类型 */
enum af_BlockType {
    parentheses = '(',  // 小括号
    brackets = '[',  // 中括号
    curly = '{',  // 大括号
};

/* 代码块创建函数 */
AFUN_CORE_EXPORT af_Code *makeElementCode(char *var, char prefix, FileLine line, FilePath path);
AFUN_CORE_EXPORT af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix,
        FileLine line, FilePath path, af_Code **next);

/* 代码块释放函数 */
AFUN_CORE_EXPORT void freeAllCode(af_Code *bt);

/* 代码块操作函数 */
AFUN_CORE_EXPORT af_Code *pushCode(af_Code **base, af_Code *next);
AFUN_CORE_EXPORT af_Code *copyCode(af_Code *base, FilePath *path);

/* 代码块属性获取函数 */
AFUN_CORE_EXPORT af_Code *getCodeNext(af_Code *bt);
AFUN_CORE_EXPORT af_Code *getCodeElement(af_Code *bt);
AFUN_CORE_EXPORT char *codeToStr(af_Code *code, int n);

/* 代码块IO函数 */
AFUN_CORE_EXPORT bool writeAllCode(af_Code *bt, FILE *file);
AFUN_CORE_EXPORT bool readAllCode(af_Code **bt, FILE *file);

#endif //AFUN_BYTECODE
