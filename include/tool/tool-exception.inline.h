#ifndef AFUN_TOOL_EXCEPTION_INLINE_H
#define AFUN_TOOL_EXCEPTION_INLINE_H

#include "tool-exception.h"

namespace aFuntool {
    inline aFunException::aFunException(std::string msg) : message{std::move(msg)} {

    }

    inline const char *aFunException::what() {
        return message.c_str();
    }

    inline aFuntoolException::aFuntoolException(const std::string &msg) : aFunException{msg} {

    }

    inline FileOpenException::FileOpenException(const FilePath &file) : aFuntoolException("File cannot open: " + file) {

    }

    inline RegexException::RegexException(const std::string &msg) : aFuntoolException("Regex error: " + msg) {

    }

    inline LogFatalError::LogFatalError(const char *msg) : aFuntoolException(msg) {

    }

    inline Exit::Exit(int exit_code_) : aFuntoolException("Exit by user"), exit_code{exit_code_} {

    }

    inline int Exit::getExitCode() const {
        return exit_code;
    }
}

#endif //AFUN_TOOL_EXCEPTION_INLINE_H
