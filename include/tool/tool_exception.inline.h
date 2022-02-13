#ifndef AFUN_TOOL_EXCEPTION_INLINE_H
#define AFUN_TOOL_EXCEPTION_INLINE_H
#ifdef __cplusplus

#include "tool_exception.h"

#ifndef AFUN_TOOL_C
namespace aFuntool {
#endif

    aFunException::aFunException(std::string msg) : message{std::move(msg)} {

    }

    const std::string &aFunException::getMessage() const {
        return message;
    }

    aFuntoolException::aFuntoolException(const std::string &msg) : aFunException{msg} {

    }

    FileOpenException::FileOpenException(const FilePath &file) : aFuntoolException("File cannot open: " + file) {

    }

    RegexException::RegexException(const std::string &msg) : aFuntoolException("Regex error: " + msg) {

    }

    LogFatalError::LogFatalError(const char *msg) : aFuntoolException(msg) {

    }

    Exit::Exit(int exit_code_) : aFuntoolException("Exit by user"), exit_code{exit_code_} {

    }

    int Exit::getExitCode() const {
        return exit_code;
    }

#ifndef AFUN_TOOL_C
}
#endif

#endif
#endif //AFUN_TOOL_EXCEPTION_INLINE_H
