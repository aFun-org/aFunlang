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

    size_t ReaderFile::readText(char *dest, size_t read_len, aFuncore::Reader::ReadMode &mode) {
        if (!no_first) {
            no_first = true;
            char ch;
            if (fread(&ch, sizeof(char), 1, file) != 1) {
                mode = read_mode_finished;
                return 0;
            }

            if (ch == (char)0xEF) {
                /* 处理BOM编码 */
                char ch_[2];
                if (fread(ch_, sizeof(char), 2, file) != 2 || ch_[0] != (char)0xBB || ch_[1] != (char)0xBF) {
                    mode = read_mode_error;
                    return 0;
                }
//                writeTrackLog(aFunRTLogger, "Parser utf-8 with BOM");
            } else {
                ungetc(ch, file);
//                writeTrackLog(aFunRTLogger, "Parser utf-8 without BOM");
            }
        }

        size_t len_r =  fread(dest, sizeof(char), read_len, file);
        if (aFuntool::clear_ferror(file)) {
            mode = read_mode_error;
        } else if (feof(file))
            mode = read_mode_finished;
        return len_r;
    }
}
