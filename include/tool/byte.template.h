#ifndef AFUN_BYTE_TEMPLATE_H
#define AFUN_BYTE_TEMPLATE_H

#include "byte.h"

namespace aFuntool {
    /**
     * 写入一个整数
     * @tparam T 整数类型
     * @param file FILE 结构体
     * @param num 整数
     * @return
     */
    template<typename T>
    bool byteWriteInt(FILE *file, T num){
        if (endian != save_as) {
            const size_t len = sizeof(T) / sizeof(uint8_t);  // NOLINT 允许 size(T) / size(T)
            union {
                T a;//元素a，占2个字节
                uint8_t b[len];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
            } in{.a = num}, out{};

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
    template<typename T>
    bool byteReadInt(FILE *file, T *num){
        size_t re = fread(num, sizeof(T), 1, file);

        if (endian != save_as) {
            const size_t len = sizeof(T) / sizeof(uint8_t);  // NOLINT 允许 size(T) / size(T)
            union {
                T a;//元素a，占2个字节
                uint8_t b[len];//元素b，占1个字节，b在内存中的地址为a最低字节的地址
            } in{.a = *num}, out{};

            for (int i = 0; i < len; i++)
                out.b[len - i] = in.b[i];  // 大小端序转换
            *num = out.a;
        }

        return re == 1;
    }
}

#endif //AFUN_BYTE_TEMPLATE_H
