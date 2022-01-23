#ifndef AFUN_BYTE_H
#define AFUN_BYTE_H
#include <cstdio>  // NOLINT
#include <iostream>
#include "aFunToolExport.h"

namespace aFuntool {
    enum EndianType {
        little_endian = 0,
        big_endian
    };

    extern enum EndianType endian;
    extern enum EndianType save_as;

    AFUN_TOOL_EXPORT void getEndian();

    template <typename T>
    AFUN_TOOL_EXPORT bool byteWriteInt(FILE *file, T num);;

    AFUN_TOOL_EXPORT bool byteWriteStr(FILE *file, const char *str);
    AFUN_TOOL_EXPORT bool byteWriteStr(FILE *file, const std::string &str);

    template <typename T>
    AFUN_TOOL_EXPORT bool byteReadInt(FILE *file, T *num);

    AFUN_TOOL_EXPORT bool byteReadStr(FILE *file, std::string &str);
    AFUN_TOOL_EXPORT bool byteReadStr(FILE *file, char *&str);

}

#include "byte.template.h"

#endif //AFUN_BYTE_H
