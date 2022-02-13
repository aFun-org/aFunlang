#ifndef AFUN_IT_READER_H
#define AFUN_IT_READER_H
#include <functional>
#include "aFunlangExport.h"
#include "aFunparser.h"
#include "init.h"

namespace aFunit {
    class AFUN_LANG_EXPORT StringReader : public aFunparser::Reader {
    public:
        AFUN_INLINE StringReader(std::string str_, const aFuntool::FilePath &path_);
        size_t readText(char *dest, size_t read_len, ReadMode &mode) override;
    private:
        std::string str;
        size_t index;
        size_t len;
    };

    class AFUN_LANG_EXPORT FileReader : public aFunparser::Reader {
    public:
        AFUN_INLINE explicit FileReader(const aFuntool::FilePath &path_) noexcept(false);
        size_t readText(char *dest, size_t read_len, ReadMode &mode) override;
    private:
        FILE *file;
        bool no_first;
    };

}

#include "it_reader.inline.h"

#endif //AFUN_IT_READER_H
