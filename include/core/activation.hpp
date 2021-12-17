#ifndef AFUN_ACTIVATION_HPP
#define AFUN_ACTIVATION_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    class Activation;
    class TopActivation;
}

#include "msg.hpp"
#include "code.hpp"
#include "inter.hpp"
#include "var.hpp"

namespace aFuncore {
    class Activation {
    protected:
        Activation *prev;
        Inter *inter;

        UpMessage *up;
        DownMessage *down;

        VarList *varlist;
        VarList *old_varlist;

        StringFilePath path;
        FileLine line;
    public:
        Activation(Inter *inter_, Activation *prev_);
        virtual ~Activation();
        virtual Code *getCode()=0;
        [[nodiscard]] StringFilePath getFilePath() const {return path;}
        [[nodiscard]] FileLine getFileLine() const {return line;}
    };

    class TopActivation {
        Code *start;
        Code *next;
    public:
        Code *getCode();
    };

}

#endif //AFUN_ACTIVATION_HPP
