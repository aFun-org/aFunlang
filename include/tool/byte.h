#ifndef AFUN_BYTE_H
#define AFUN_BYTE_H
#include "aFunToolExport.h"

#ifdef __cplusplus
#include <cstdio>  // NOLINT
#include <iostream>
#else
#include <stdio.h>
#endif

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    enum EndianType {
        little_endian = 0,
        big_endian
    };

    AFUN_TOOL_C_EXPORT_VAR enum EndianType endian;
    AFUN_TOOL_C_EXPORT_VAR enum EndianType save_as;

    AFUN_TOOL_C_EXPORT_FUNC void getEndian();

#ifdef __cplusplus
    template <typename T>
    bool byteWriteInt(FILE *file, T num);
#endif

    AFUN_TOOL_C_EXPORT_FUNC bool byteWriteStr(FILE *file, const char *str);

#ifdef __cplusplus
    AFUN_TOOL_EXPORT bool byteWriteStr(FILE *file, const std::string &str);
#endif

#ifdef __cplusplus
    template <typename T>
    bool byteReadInt(FILE *file, T *num);
#endif

AFUN_TOOL_EXPORT bool byteReadStr(FILE *file, char **str);
#ifdef __cplusplus
    AFUN_TOOL_C_EXPORT_FUNC bool byteReadStr(FILE *file, std::string &str);
#endif

#ifndef AFUN_TOOL_C
}
#endif

#include "byte.template.h"

#endif //AFUN_BYTE_H
