#ifndef AFUN_IT_READER_INLINE_H
#define AFUN_IT_READER_INLINE_H
#include "it-reader.h"
#include "it-exception.h"

namespace aFunit {
    StringReader::StringReader(std::string str_, const aFuntool::FilePath &path_)
        : Reader{path_, 0}, str{std::move(str_)}, index{0}, len{str.size()} {

    }

    FileReader::FileReader(const aFuntool::FilePath &path_)
        : Reader{path_, 0}, file{aFuntool::fileOpen(path_, "rb")}, no_first{false} {
        if (file == nullptr)
            throw readerFileOpenError(path_);
    }
}

#endif //AFUN_IT_READER_INLINE_H
