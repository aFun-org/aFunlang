#ifndef AFUN__BYTECODE_H_PUBLIC
#define AFUN__BYTECODE_H_PUBLIC
#include <stdio.h>

typedef struct af_Code af_Code;

/* 括号类型 */
enum af_BlockType {
    parentheses = 0,  // 小括号
    brackets,  // 中括号
    curly,  // 大括号
};

/* 代码块创建函数 */
af_Code *makeElementCode(char *var, char prefix, FileLine line, FilePath path);
af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix, FileLine line, FilePath path, af_Code **next);

/* 代码块释放函数 */
bool freeCodeWithElement(af_Code *bt, af_Code **next);
void freeAllCode(af_Code *bt);

/* 代码块操作函数 */
af_Code *connectCode(af_Code **base, af_Code *next);
af_Code *copyCode(af_Code *base, FilePath *path);

/* 代码块属性获取函数 */
bool getCodeBlockNext(af_Code *bt, af_Code **next);
void printCode(af_Code *bt);

/* 代码块IO函数 */
bool writeAllCode(af_Code *bt, FILE *file);
bool readAllCode(af_Code **bt, FILE *file);

#endif //AFUN__BYTECODE_H_PUBLIC
