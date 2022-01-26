#ifndef AFUN_READER_H
#define AFUN_READER_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class Reader {
    public:
        typedef enum ReadMode {
            read_mode_normal = 0,
            read_mode_finished = 1,
            read_mode_error = 2,
        } ReadMode;

        static const size_t DEFAULT_BUF_SIZE = 1024;
        static const size_t NEW_BUF_SIZE  = 512;

        explicit inline Reader(aFuntool::FilePath path_, aFuntool::FileLine line_ = 1);
        Reader(const Reader &) = delete;
        virtual ~Reader();
        Reader &operator=(const Reader &) = delete;

        virtual size_t readText(char *dest, size_t len, ReadMode &mode) = 0;

        [[nodiscard]] inline aFuntool::FileLine getFileLine() const;
        [[nodiscard]] inline const aFuntool::FilePath &getFilePath() const;
        char *readWord(size_t del_index);
        void readFirstWord();
        char getChar();

    private:
        char *buf;
        size_t buf_size;  // buf的长度-1
        char *read;
        bool read_end;
        bool read_error;

        aFuntool::FileLine line;
        aFuntool::FilePath path;
    };
}

#include "reader.inline.h"

#endif //AFUN_READER_H
