#ifndef AFUN_EXCEPTION_HPP
#define AFUN_EXCEPTION_HPP
#include "iostream"

namespace aFuncore {
    class AttributesError : public std::exception {
        std::string msg;
    public:
        explicit AttributesError(const std::string &attributes) {msg = (std::string("Get attributes error: ") + attributes);}
        virtual const char *what() {return msg.c_str();}
    };
}

#endif //AFUN_EXCEPTION_HPP
