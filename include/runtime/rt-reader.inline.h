#ifndef AFUN_RT_READER_INLINE_H
#define AFUN_RT_READER_INLINE_H
#include "rt-reader.h"
#include "rt-exception.h"

namespace aFunrt {
    inline StringReader::StringReader(std::string str_, const aFuntool::FilePath &path_)
        : Reader{path_, 0}, str{std::move(str_)} {
        index = 0;
        len = str.size();
    }

    inline FileReader::FileReader(const aFuntool::FilePath &path_)
        : Reader{path_, 0} {
        file = aFuntool::fileOpen(path_, "rb");
        if (file == nullptr)
            throw readerFileOpenError(path_);
        no_first = false;
    }
}

#endif //AFUN_RT_READER_INLINE_H
