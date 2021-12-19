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

        VarList *varlist;
        VarList *old_varlist;

        UpMessage *up;
        DownMessage *down;
    public:
        Inter *const inter;

        StringFilePath path;
        FileLine line;

        explicit Activation(Inter *inter_);
        virtual ~Activation();

        virtual Code *getCode()=0;
        virtual bool onTail()=0;

        [[nodiscard]] VarList *getVarlist() const {return varlist;}
        [[nodiscard]] Activation *toPrev() const {return prev;}
        [[nodiscard]] UpMessage *getUpStream() const {return up;}
        [[nodiscard]] DownMessage *getDownStream() const {return down;}
    };

    class TopActivation : public Activation {
        Code *start;
        Code *next;
    public:
        explicit TopActivation(Code *code, Inter *inter_);
        ~TopActivation() override;

        Code *getCode() override;
        bool onTail() override {return false;}
    };
}

#endif //AFUN_ACTIVATION_HPP
