#ifndef AFUN_ACTIVATION_HPP
#define AFUN_ACTIVATION_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    class Activation;
    class TopActivation;

    typedef enum ActivationStatus {
        as_run = 0,
        as_end = 1,
    } ActivationStatus;
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

        virtual ActivationStatus getCode(Code *&code)=0;
        virtual bool onTail()=0;
        virtual void runCode(Code *code);

        [[nodiscard]] VarList *getVarlist() const {return varlist;}
        [[nodiscard]] Activation *toPrev() const {return prev;}
        [[nodiscard]] UpMessage *getUpStream() const {return up;}
        [[nodiscard]] DownMessage *getDownStream() const {return down;}
    };

    class ExeActivation : public Activation {
        Code *start;
        Code *next;
        bool first=true;
    public:
        explicit ExeActivation(Code *code, Inter *inter_) : Activation(inter_), start{code}, next{code} {}
        ActivationStatus getCode(Code *&code) override;
        bool onTail() override {return next == nullptr;}
        [[nodiscard]] Code *getStart() const {return start;}
    };

    class TopActivation : public ExeActivation {
    public:
        explicit TopActivation(Code *code, Inter *inter_);
        ~TopActivation() override;
        bool onTail() override {return false;}
    };
}

#endif //AFUN_ACTIVATION_HPP
