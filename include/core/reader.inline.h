#ifndef AFUN_READER_INLINE_H
#define AFUN_READER_INLINE_H
#include "reader.h"

namespace aFuncore {
    inline Reader::Reader(aFuntool::FilePath path_, aFuntool::FileLine line_)
            : path{std::move(path_)}, line{line_}, read_end{false}, read_error{false} {
        buf = aFuntool::safeCalloc<char>(DEFAULT_BUF_SIZE + 1);
        buf_size = DEFAULT_BUF_SIZE;  // buf_size 不包括NUL
        read = buf;
    }

    size_t Reader::countRead() const {
        return read - buf;
    }

    bool Reader::isEnd() const {
        return read_end;
    }

    bool Reader::isError() const {
        return read_error;
    }

    aFuntool::FileLine Reader::getFileLine() const {
        return line;
    }

    const aFuntool::FilePath &Reader::getFilePath() const {
        return path;
    }
}

#endif //AFUN_READER_INLINE_H
