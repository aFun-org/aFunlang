#ifndef AFUN__BYTECODE_H_PUBLIC
#define AFUN__BYTECODE_H_PUBLIC
#include <stdio.h>

typedef struct af_Code af_Code;

enum af_BlockType {
    parentheses = 0,  // 小括号
    brackets,  // 中括号
    curly,  // 大括号
};

af_Code *makeLiteralCode(char *literal_data, char *func, char prefix, FileLine line, FilePath path);
af_Code *makeVariableCode(char *var, char prefix, FileLine line, FilePath path);
af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix, FileLine line, FilePath path, af_Code **next);
af_Code *connectCode(af_Code **base, af_Code *next);
af_Code *copyCode(af_Code *base, FilePath *path);
af_Code *freeCode(af_Code *bt);
bool freeCodeWithElement(af_Code *bt, af_Code **next);
void freeAllCode(af_Code *bt);
bool writeAllCode(af_Code *bt, FILE *file);
bool readAllCode(af_Code **bt, FILE *file);
void printCode(af_Code *bt);

#endif //AFUN__BYTECODE_H_PUBLIC
