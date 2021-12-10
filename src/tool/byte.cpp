#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "tool.hpp"
using namespace aFuntool;

namespace aFuntool {
    enum af_EndianType endian = little_endian;
    enum af_EndianType save_as = little_endian;  // 默认以小端序存储
}

/**
 * 获取机器字节序
 */
void aFuntool::getEndian() {
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

/**
 * 写入一个整数
 * @tparam T 整数类型
 * @param file FILE 结构体
 * @param num 整数
 * @return
 */
template <typename T>
bool aFuntool::byteWriteInt(FILE *file, T num) {
    if (endian != save_as) {
        const size_t len = sizeof(T) / sizeof(uint8_t);
        union {
            T a;//元素a，占2个字节
            uint8_t b[len];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = num}, out {};

        for (int i = 0; i < len; i++)
            out.b[len - i] = in.b[i];  // 大小端序转换
        num = out.a;
    }

    return fwrite(&num, sizeof(T), 1, file) == 1;
}

/**
 * 读取一个整数
 * @tparam T 整数类型
 * @param file FILE 结构体
 * @param num 整数
 * @return
 */
template <typename T>
bool aFuntool::byteReadInt(FILE *file, T *num) {
    size_t re = fread(num, sizeof(T), 1, file);

    if (endian != save_as) {
        const size_t len = sizeof(T) / sizeof(uint8_t);
        union {
            T a;//元素a，占2个字节
            uint8_t b[len];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
        } in {.a = *num}, out {};

        for (int i = 0; i < len; i++)
            out.b[len - i] = in.b[i];  // 大小端序转换
        *num = out.a;
    }

    return re == 1;
}

template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, int8_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, int16_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, int32_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, int64_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, uint8_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, uint16_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, uint32_t num);
template AFUN_TOOL_EXPORT bool aFuntool::byteWriteInt(FILE *file, uint64_t num);

template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt<int8_t>(FILE *file, int8_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, int16_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, int32_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, int64_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, uint8_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, uint16_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, uint32_t *num);
template AFUN_TOOL_EXPORT bool aFuntool::byteReadInt(FILE *file, uint64_t *num);

/**
 * 写入一个C风格字符串
 */
bool aFuntool::byteWriteStr(FILE *file, const char *str) {
    if (!byteWriteInt<uint16_t>(file, (uint16_t)strlen(str)))
        return false;
    return fwrite(str, sizeof(char), strlen(str), file) == strlen(str);
}


/**
 * 写入一个C++风格字符串
 */
bool aFuntool::byteWriteStr(FILE *file, const std::string &str) {
    size_t size = str.size();
    if (!byteWriteInt<uint16_t>(file, (uint16_t)size))
        return false;
    return fwrite(str.c_str(), sizeof(char), size, file) == size;
}


/**
 * 读取一个C风格字符串
 */
bool aFuntool::byteReadStr(FILE *file, char *&str) {
    uint16_t len;
    if (!byteReadInt<uint16_t>(file, &len))
        return false;

    str = calloc(len + 1, char);
    return fread(str, sizeof(char), len, file) == len;
}

/**
 * 读取一个C++风格字符串
 */
bool aFuntool::byteReadStr(FILE *file, std::string &str) {
    uint16_t len;
    if (!byteReadInt<uint16_t>(file, &len))
        return false;

    char *tmp = calloc(len + 1, char);
    size_t ret = fread(tmp, sizeof(char), len, file);
    str = tmp;
    free(tmp);
    return ret == len;
}
