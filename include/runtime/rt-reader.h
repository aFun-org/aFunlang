#ifndef AFUN_RT_READER_H
#define AFUN_RT_READER_H
#include <functional>
#include "aFunlangExport.h"
#include "aFuncore.h"
#include "rt-init.h"

namespace aFunrt {
    class AFUN_LANG_EXPORT StringReader : public aFuncore::Reader {
    public:
        inline StringReader(std::string str_, const aFuntool::FilePath &path_);
        size_t readText(char *dest, size_t read_len, ReadMode &mode) override;
    private:
        std::string str;
        size_t index;
        size_t len;
    };

    class AFUN_LANG_EXPORT FileReader : public aFuncore::Reader {
    public:
        inline explicit FileReader(const aFuntool::FilePath &path_) noexcept(false);
        size_t readText(char *dest, size_t read_len, ReadMode &mode) override;
    private:
        FILE *file;
        bool no_first;
    };

}

#include "rt-reader.inline.h"

#endif //AFUN_RT_READER_H
