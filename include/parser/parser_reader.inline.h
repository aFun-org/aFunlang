#ifndef AFUN_PARSER_READER_INLINE_H
#define AFUN_PARSER_READER_INLINE_H
#include "parser_reader.h"

namespace aFunparser {
    Reader::Reader(aFuntool::FilePath path_, aFuntool::FileLine line_)
            : buf{aFuntool::safeCalloc<char>(DEFAULT_BUF_SIZE + 1)}, buf_size{DEFAULT_BUF_SIZE}, read{buf}, read_end{false},
              read_error{false}, line{line_}, path{std::move(path_)} {

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

#endif //AFUN_PARSER_READER_INLINE_H
