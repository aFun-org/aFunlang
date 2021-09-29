#ifndef AFUN_BYTECODE_H
#define AFUN_BYTECODE_H

AFUN_CORE_EXPORT extern char const * const writeByteCodeError[5];
AFUN_CORE_EXPORT extern char const * const readByteCodeError[7];

AFUN_CORE_EXPORT int readByteCode(af_Code **code, FilePath path);
AFUN_CORE_EXPORT int writeByteCode(af_Code *code, FilePath path);

#endif //AFUN_BYTECODE_H
