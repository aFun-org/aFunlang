#ifndef AFUN_TOOL_EXCEPTION_H
#define AFUN_TOOL_EXCEPTION_H
#include "aFunToolExport.h"
#include "macro.h"
#include "tool-type.h"

namespace aFuntool {
    class AFUN_TOOL_EXPORT aFunException : public std::exception {
        std::string message;
    public:
        AFUN_INLINE explicit aFunException(std::string msg);
        virtual const char *what();
        [[nodiscard]] AFUN_INLINE const std::string &getMessage() const;
    };

    class aFuntoolException : public aFunException {
    public:
        AFUN_INLINE explicit aFuntoolException(const std::string &msg);
    };

    class FileOpenException : public aFuntoolException {
    public:
        AFUN_INLINE explicit FileOpenException(const FilePath &file);
    };

    class RegexException : public aFuntoolException {
    public:
        AFUN_INLINE explicit RegexException(const std::string &msg);
    };

    class LogFatalError : public aFuntoolException {
    public:
        AFUN_INLINE explicit LogFatalError(const char *msg);
    };

    class Exit : public aFuntoolException {
        int exit_code;
    public:
        AFUN_INLINE explicit Exit(int exit_code_);
        AFUN_INLINE int getExitCode() const;
    };
}

#include "tool-exception.inline.h"

#endif //AFUN_TOOL_EXCEPTION_H
