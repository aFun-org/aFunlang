#ifndef AFUN__BYTECODE_H_PUBLIC
#define AFUN__BYTECODE_H_PUBLIC
typedef struct af_ByteCode af_ByteCode;

enum af_BlockType {
    parentheses = 0,  // 小括号
    brackets,  // 中括号
    curly,  // 大括号
};

af_ByteCode *makeLiteralByteCode(char *literal_data, char *func, char prefix, FileLine line, FilePath path);
af_ByteCode *makeVariableByteCode(char *var, char prefix, FileLine line, FilePath path);
af_ByteCode *makeBlockByteCode(enum af_BlockType type, af_ByteCode *element, char prefix, FileLine line, FilePath path, af_ByteCode **next);
af_ByteCode *connectByteCode(af_ByteCode **base, af_ByteCode *next);
af_ByteCode *copyByteCode(af_ByteCode *base, FilePath *path);
af_ByteCode *freeByteCode(af_ByteCode *bt);
bool freeByteCodeWithElement(af_ByteCode *bt, af_ByteCode **next);
void freeAllByteCode(af_ByteCode *bt);
bool writeAllByteCode(af_ByteCode *bt, FILE *file);
bool readAllByteCode(af_ByteCode **bt, FILE *file);
void printByteCode(af_ByteCode *bt);

#endif //AFUN__BYTECODE_H_PUBLIC
