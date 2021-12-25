#ifndef AFUN_ACTIVATION_HPP
#define AFUN_ACTIVATION_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "core.hpp"
#include "value.hpp"

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

    class FuncActivation : public Activation {
        enum {
            func_first = 0,
            func_get_func = 1,
            func_get_arg = 2,
        } status = func_first;

        bool on_tail = false;
        Code *call;

        Function *func = nullptr;
        Function::CallFunction *call_func = nullptr;

        std::list<Function::CallFunction::ArgCodeList> *acl = nullptr;
        std::list<Function::CallFunction::ArgCodeList>::iterator acl_begin;
        std::list<Function::CallFunction::ArgCodeList>::iterator acl_end;
    public:
        explicit FuncActivation(Code *code, Inter *inter_) : Activation(inter_), call{code,} {}
        ~FuncActivation() override;
        ActivationStatus getCode(Code *&code) override;
        bool onTail() override {return on_tail;}
    };
}

#endif //AFUN_ACTIVATION_HPP
