#include "tool-type.h"
#include "encoding.h"

namespace aFuntool {
    /**
     * 检查给定字符串是否utf-8编码
     * @param str 字符串
     */
    bool isCharUTF8(const char *str) {
        int code = 0;  // utf-8 多字节数
        for (const char *ch = str; *ch != NUL; ch++) {
            unsigned char c = *ch;
            unsigned char c_ = ~c;

            assertFatalErrorLog(code >= 0 && code <= 5, aFunSysLogger, 2, "str = %s", str);
            if (code == 0) {
                if ((c_ & 0xFC) == 0 && (c & 0x02) == 0)  // 检查是否为1111110x, 先对其取反, 使用0xFC掩码检查前6位是否为0, 然后单独检查倒数第二位是否为0
                    code = 5;  // 剩余 5 个字节
                else if ((c_ & 0xF8) == 0 && (c & 0x04) == 0)
                    code = 4;  // 剩余 4 个字节
                else if ((c_ & 0xF0) == 0 && (c & 0x08) == 0)
                    code = 3;  // 剩余 3 个字节
                else if ((c_ & 0xE0) == 0 && (c & 0x10) == 0)
                    code = 2;  // 剩余 2 个字节
                else if ((c_ & 0xC0) == 0 && (c & 0x20) == 0)
                    code = 1;  // 剩余 1 个字节
                else if ((c & 0x80) == 0)  // 检查最高位是否为0
                    code = 0;
                else
                    return false;
            } else if ((c_ & 0x80) == 0 && (c & 0x40) == 0)
                code--;
            else
                return false;
        }

        return true;
    }
}