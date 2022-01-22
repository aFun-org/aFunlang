#ifndef AFUN_ACTIVATION_H
#define AFUN_ACTIVATION_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"
#include "value.h"
#include "msg.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Activation {
    protected:
        Activation *prev;

        VarList *varlist;

        UpMessage up;
        DownMessage down;

        StringFilePath path;
        FileLine line;

        virtual void runCodeElement(Code *code);
        virtual void runCodeBlockP(Code *code);
        virtual void runCodeBlockC(Code *code);
        virtual void runCodeBlockB(Code *code);
    public:
        Inter &inter;

        template <typename Callable,typename...T>
        static void forEach(Activation *activation, Callable func, T...arg);

        explicit Activation(Inter &inter_);
        virtual ~Activation();
        Activation &operator=(const Activation &)=delete;

        virtual ActivationStatus getCode(Code *&code) = 0;
        virtual void runCode(Code *code);
        virtual inline void endRun();

        [[nodiscard]] inline VarList *getVarlist() const;
        [[nodiscard]] inline Activation *toPrev() const;
        [[nodiscard]] inline UpMessage &getUpStream();
        [[nodiscard]] inline DownMessage &getDownStream();

        [[nodiscard]] inline FileLine getFileLine() const;
        [[nodiscard]] inline  const StringFilePath &getFilePath() const;
    };

    class AFUN_CORE_EXPORT ExeActivation : public Activation {
        Code *start;
        Code *next;
        bool first=true;
    public:
        explicit inline  ExeActivation(Code *code, Inter &inter_);
        ActivationStatus getCode(Code *&code) override;
        [[nodiscard]] inline  Code *getStart() const;
    };

    class AFUN_CORE_EXPORT TopActivation : public ExeActivation {
    public:
        explicit TopActivation(Code *code, Inter &inter_);
        ~TopActivation() override;
    };

    class AFUN_CORE_EXPORT FuncActivation : public Activation {
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
        explicit inline FuncActivation(Code *code, Inter &inter_);
        ~FuncActivation() override;
        ActivationStatus getCode(Code *&code) override;
        void endRun() override;
    };
}

#include "activation.inline.h"
#include "activation.template.h"

#endif //AFUN_ACTIVATION_H
