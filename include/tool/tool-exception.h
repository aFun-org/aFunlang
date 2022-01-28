#ifndef AFUN_TOOL_EXCEPTION_H
#define AFUN_TOOL_EXCEPTION_H
#include "tool.h"

namespace aFuntool {
    class aFunException : public std::exception {
        std::string message;
    public:
        inline explicit aFunException(std::string msg);
        inline virtual const char *what();
    };

    class aFuntoolException : public aFunException {
    public:
        inline explicit aFuntoolException(const std::string &msg);
    };

    class FileOpenException : public aFuntoolException {
    public:
        inline explicit FileOpenException(const FilePath &file);
    };

    class RegexException : public aFuntoolException {
    public:
        inline explicit RegexException(const std::string &msg);
    };

    class LogFatalError : public aFuntoolException {
    public:
        inline explicit LogFatalError(const char *msg);
    };

    class Exit : public aFuntoolException {
        int exit_code;
    public:
        inline explicit Exit(int exit_code_);
        inline int getExitCode() const;
    };
}

#include "tool-exception.inline.h"

#endif //AFUN_TOOL_EXCEPTION_H
