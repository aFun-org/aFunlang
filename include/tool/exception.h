#ifndef AFUN_EXCEPTION_H
#define AFUN_EXCEPTION_H
#include "macro.h"

namespace aFuntool {
    class FileOpenException : public std::exception {
        std::string msg;
    public:
        explicit FileOpenException(ConstFilePath file) {msg = std::string("File cannot open") + file;}
        virtual const char *what() {return "File cannot open";}
    };

    class RegexException : public std::exception
    {
        std::string message;
    public:
        explicit RegexException(const std::string &msg) { this->message = "Regex Error: " + msg;}
        virtual const char *what() {return message.c_str();}
    };

    class LogFatalError : public std::exception {
        std::string message;
    public:
        explicit LogFatalError(const char *msg) {message = msg;}
        virtual const char *what() {return message.c_str();}
    };
}

#endif //AFUN_EXCEPTION_H
