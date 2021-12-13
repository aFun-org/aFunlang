/*
 * 文件名: tool.h
 * 目标: aFun tool公共API
 * aFunTool是aFun实用工具库, 内含aFun调用的实用函数
 */

#ifndef AFUN_TOOL_HPP
#define AFUN_TOOL_HPP

#include "macro.hpp"
#include "aFunToolExport.h"

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

#include "mem.hpp"

#include "stdio_.hpp"
#include "exit_.hpp"
#include "btye.hpp"
#include "dlc.hpp"
#include "file.hpp"
#include "hash.hpp"
#include "md5.hpp"
#include "path.hpp"
#include "regex.hpp"
#include "str.hpp"
#include "time_.hpp"
#include "log.hpp"

#include "pthread.h"
#include "fflags.h"
#include "aFun_ht.h"

#endif //AFUN_TOOL_HPP
