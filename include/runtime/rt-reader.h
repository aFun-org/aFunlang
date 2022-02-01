#ifndef AFUN_RT_READER_H
#define AFUN_RT_READER_H
#include "aFuncore.h"

namespace aFunrt {
    class ReaderString : public aFuncore::Reader {
    public:
        inline ReaderString(std::string str_, const aFuntool::FilePath &path_);

        size_t readText(char *dest, size_t read_len, ReadMode &mode) override;
    private:
        std::string str;
        size_t index;
        size_t len;
    };
}

#include "rt-reader.inline.h"

#endif //AFUN_RT_READER_H
