#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "tool.h"
#include "byte.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    enum EndianType endian = little_endian;
    enum EndianType save_as = little_endian;  // 默认以小端序存储

    /**
     * 获取机器字节序
     */
    void getEndian(){
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
     * 写入一个C风格字符串
     */
    bool byteWriteStr(FILE *file, const char *str){
        if (!byteWriteInt<uint16_t>(file, static_cast<uint16_t>(strlen(str))))
            return false;
        return fwrite(str, sizeof(char), strlen(str), file) == strlen(str);
    }


    /**
     * 写入一个C++风格字符串
     */
    bool byteWriteStr(FILE *file, const std::string &str){
        size_t size = str.size();
        if (!byteWriteInt<uint16_t>(file, static_cast<uint16_t>(size)))
            return false;
        return fwrite(str.c_str(), sizeof(char), size, file) == size;
    }


    /**
     * 读取一个C风格字符串
     */
    bool byteReadStr(FILE *file, char **str){
        uint16_t len;
        if (!byteReadInt<uint16_t>(file, &len))
            return false;

        if (len == 0) {
            *str = nullptr;
            return true;
        }

        *str = safeCalloc<char>(len + 1);
        return fread(*str, sizeof(char), len, file) == len;
    }

    /**
     * 读取一个C++风格字符串
     */
    bool byteReadStr(FILE *file, std::string &str){
        uint16_t len;
        if (!byteReadInt<uint16_t>(file, &len))
            return false;

        if (len == 0) {
            str = "";
            return true;
        }

        char *tmp = safeCalloc<char>(len + 1);
        size_t ret = fread(tmp, sizeof(char), len, file);
        str = tmp;
        safeFree(tmp);
        return ret == len;
    }

#ifndef AFUN_TOOL_C
}
#endif
