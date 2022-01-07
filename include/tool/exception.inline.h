#ifndef AFUN_EXCEPTION_INLINE_H
#define AFUN_EXCEPTION_INLINE_H

#include "exception.h"

namespace aFuntool {
    inline FileOpenException::FileOpenException(ConstFilePath file) {
        this->message = std::string("File cannot open: ") + file;
    }

    inline const char *FileOpenException::what() {
        return message.c_str();
    }

    inline RegexException::RegexException(const std::string &msg) {
        this->message = "Regex error: " + msg;
    }

    inline const char *RegexException::what() {
        return message.c_str();
    }

    inline LogFatalError::LogFatalError(const char *msg) {
        this->message = msg;
    }

    inline const char *LogFatalError::what() {
        return message.c_str();
    }
}

#endif //AFUN_EXCEPTION_INLINE_H
