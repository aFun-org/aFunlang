#ifndef AFUN_TOOL_EXCEPTION_H
#define AFUN_TOOL_EXCEPTION_H
#include "tool.h"

namespace aFuntool {
    class FileOpenException : public std::exception {
        std::string message;
    public:
        inline explicit FileOpenException(ConstFilePath file);
        inline virtual const char *what();
    };

    class RegexException : public std::exception
    {
        std::string message;
    public:
        inline explicit RegexException(const std::string &msg);
        inline virtual const char *what();
    };

    class LogFatalError : public std::exception {
        std::string message;
    public:
        inline explicit LogFatalError(const char *msg);
        inline virtual const char *what();
    };
}

#include "tool-exception.inline.h"

#endif //AFUN_TOOL_EXCEPTION_H
