#ifndef AFUN_EXCEPTION_H
#define AFUN_EXCEPTION_H
#include "tool.h"

namespace aFuntool {
    class FileOpenException : public std::exception {
        std::string message;
    public:
        explicit FileOpenException(ConstFilePath file);
        virtual const char *what();
    };

    class RegexException : public std::exception
    {
        std::string message;
    public:
        explicit RegexException(const std::string &msg);
        virtual const char *what();
    };

    class LogFatalError : public std::exception {
        std::string message;
    public:
        explicit LogFatalError(const char *msg);
        virtual const char *what();
    };
}

#include "exception.inline.h"

#endif //AFUN_EXCEPTION_H
