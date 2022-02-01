#include "rt-reader.h"

namespace aFunrt {
    size_t ReaderString::readText(char *dest, size_t read_len, ReadMode &mode) {
        if (index == len)  // 读取到末尾
            return 0;

        if (index + read_len > len) {  // 超出长度范围
            read_len = len - index;
            mode = read_mode_finished;
        }

        memcpy(dest, str.c_str() + index, read_len);
        index += read_len;
        return read_len;
    }
}
