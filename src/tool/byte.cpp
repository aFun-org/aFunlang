#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include "tool.hpp"

enum af_EndianType endian = little_endian;
enum af_EndianType save_as = little_endian;  // 默认以小端序存储

/*
 * 函数名: getEndian
 * 目标: 获取机器字节序
 */
void getEndian() {
    union {
        int16_t a;//元素a，占2个字节
        int8_t b;//元素b，占1个字节，b在内存中的地址为a最低字节的地址
    } test = {.a = 0x1234};

    if (test.b == 0x34)
        endian = little_endian;
    else if (test.b == 0x12)
        endian = big_endian;
    else
        abort();
}

bool byteWriteUint_8(FILE *file, uint8_t ch) {
    return fwrite(&ch, sizeof(uint8_t), 1, file) == 1;
}

bool byteWriteUint_16(FILE *file, uint16_t num) {
    if (endian != save_as) {
        union {
            uint16_t a;//元素a，占2个字节
            uint8_t b[2];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = num}, out {};

        out.b[1] = in.b[0];
        out.b[0] = in.b[1];
        num = out.a;
    }

    return fwrite(&num, sizeof(uint16_t), 1, file) == 1;
}

bool byteWriteUint_32(FILE *file, uint32_t num) {
    if (endian != save_as) {
        union {
            uint32_t a;//元素a，占2个字节
            uint8_t b[4];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = num}, out {};

        out.b[3] = in.b[0];
        out.b[2] = in.b[1];
        out.b[1] = in.b[2];
        out.b[0] = in.b[3];
        num = out.a;
    }

    return fwrite(&num, sizeof(uint32_t), 1, file) == 1;
}

bool byteWriteUint_64(FILE *file, uint64_t num) {
    if (endian != save_as) {
        union {
            uint64_t a;//元素a，占2个字节
            uint8_t b[8];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = num}, out {};

        out.b[7] = in.b[0];
        out.b[6] = in.b[1];
        out.b[5] = in.b[2];
        out.b[4] = in.b[3];
        out.b[3] = in.b[4];
        out.b[2] = in.b[5];
        out.b[1] = in.b[6];
        out.b[0] = in.b[7];
        num = out.a;
    }

    return fwrite(&num, sizeof(uint64_t), 1, file) == 1;
}

bool byteReadUint_8(FILE *file, uint8_t *ch) {
    return fread(ch, sizeof(uint8_t), 1, file) == 1;
}

bool byteReadUint_16(FILE *file, uint16_t *num) {
    size_t re = fread(num, sizeof(uint16_t), 1, file);

    if (endian != save_as) {
        union {
            uint16_t a;//元素a，占2个字节
            uint8_t b[2];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = *num}, out {};

        out.b[1] = in.b[0];
        out.b[0] = in.b[1];
        *num = out.a;
    }

    return re == 1;
}

bool byteReadUint_32(FILE *file, uint32_t *num) {
    size_t re = fread(num, sizeof(uint32_t), 1, file);

    if (endian != save_as) {
        union {
            uint32_t a;//元素a，占2个字节
            uint8_t b[4];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = *num}, out {};

        out.b[3] = in.b[0];
        out.b[2] = in.b[1];
        out.b[1] = in.b[2];
        out.b[0] = in.b[3];
        *num = out.a;
    }

    return re == 1;
}

bool byteReadUint_64(FILE *file, uint64_t *num) {
    size_t re = fread(num, sizeof(uint64_t), 1, file);

    if (endian != save_as) {
        union {
            uint64_t a;//元素a，占2个字节
            uint8_t b[8];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = *num}, out {};

        out.b[7] = in.b[0];
        out.b[6] = in.b[1];
        out.b[5] = in.b[2];
        out.b[4] = in.b[3];
        out.b[3] = in.b[4];
        out.b[2] = in.b[5];
        out.b[1] = in.b[6];
        out.b[0] = in.b[7];
        *num = out.a;
    }

    return re == 1;
}

bool byteWriteStr(FILE *file, const char *str) {
    if (!byteWriteUint_16(file, strlen(str)))
        return false;
    return fwrite(str, sizeof(char), strlen(str), file) == strlen(str);
}

bool byteReadStr(FILE *file, char **str) {
    uint16_t len;
    if (!byteReadUint_16(file, &len))
        return false;

    *str = calloc(len + 1, char);
    return fread(*str, sizeof(char), len, file) == len;
}

