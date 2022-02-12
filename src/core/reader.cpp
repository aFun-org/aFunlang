#include "core-logger.h"
#include "reader.h"

namespace aFuncore {
    Reader::~Reader() {
        aFuntool::safeFree(buf);
    }

    char *Reader::readWord(size_t del_index) {
        char *re;
        ReadMode mode = read_mode_normal;
        read = buf;  // 重置指针

        if (del_index == 0)
            return aFuntool::strCopy("");  // 返回空字符串

        re = aFuntool::safeCalloc<char>(del_index + 1);
        memcpy(re, buf, del_index);  // 复制旧字符串
        memmove(buf, buf + del_index, buf_size - del_index + 1);  // +1是为了涵盖NUL

        if (!read_end) { // 没到尾部, 则写入数据
            char *write = buf + strlen(buf);  // 数据写入的位置
            size_t len_ = buf_size - strlen(buf);
            size_t len = readText(write, len_, mode);
            if (len > len_)
                len = len_;
            *(write + len) = aFuntool::NUL;
        }

        if (mode == read_mode_finished)
            read_end = true;
        else if (mode == read_mode_error) {
            read_end = true;
            read_error = true;
        }

        /* 计算行号 */
        for (char *tmp = re; *tmp != aFuntool::NUL; tmp ++) {
            if (*tmp == '\n')
                line++;
        }

        if (!aFuntool::isCharUTF8(re)) {
            aFuntool::safeFree(re);
            errorLog(aFunCoreLogger, "Is not utf-8");
            return nullptr;
        }

        return re;
    }

    void Reader::readFirstWord() {
        ReadMode mode = read_mode_normal;
        read = buf;  // 重置指针

        char *write = buf + strlen(buf);  // 数据写入的位置
        size_t len_ = buf_size - strlen(buf);
        size_t len = readText(write, len_, mode);
        if (len > len_)
            len = len_;
        *(write + len) = aFuntool::NUL;

        if (mode == read_mode_finished)
            read_end = true;
        else if (mode == read_mode_error) {
            read_end = true;
            read_error = true;
        }
    }

    char Reader::getChar() {
        char ch = *read;
        if (ch != aFuntool::NUL) {  // 未读取到末尾
            read++;
            return ch;
        } else if (read_end)  // 读取到末尾, 且无新内容
            return aFuntool::NUL;

        if (read == buf + buf_size) {
            char *new_buf = aFuntool::safeCalloc<char>(buf_size + NEW_BUF_SIZE + 1);
            memcpy(new_buf, buf, buf_size);

            ReadMode mode = read_mode_normal;
            size_t len = readText(new_buf + buf_size, NEW_BUF_SIZE, mode);
            if (len > NEW_BUF_SIZE)
                len = NEW_BUF_SIZE;
            *(new_buf + buf_size + len) = aFuntool::NUL;

            if (mode == read_mode_finished)
                read_end = true;
            else if (mode == read_mode_error) {
                read_end = true;
                read_error = true;
            }

            aFuntool::safeFree(buf);
            buf = new_buf;
            buf_size = buf_size + NEW_BUF_SIZE;
            read = buf + buf_size - NEW_BUF_SIZE;  // 当前读取的位置
        } else {
            ReadMode mode = read_mode_normal;
            size_t len_ = buf_size - (read - buf);  // 总长度  - (已读取长度) = 剩余空白
            size_t len = readText(read, len_, mode);
            if (len > len_)
                len = len_;
            *(read + len) = aFuntool::NUL;
        }

        ch = *read;
        if (ch != aFuntool::NUL)
            read++;
        return ch;
    }
}