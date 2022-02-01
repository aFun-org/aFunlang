#ifndef AFUN_RT_READER_INLINE_H
#define AFUN_RT_READER_INLINE_H
#include "rt-reader.h"

namespace aFunrt {
    inline ReaderString::ReaderString(std::string str_, const aFuntool::FilePath &path_)
        : Reader{path_, 0}, str{std::move(str_)} {
        index = 0;
        len = str.size();
    }
}

#endif //AFUN_RT_READER_INLINE_H
