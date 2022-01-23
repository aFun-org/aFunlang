#ifndef AFUN_CORE_ACTIVATION_H
#define AFUN_CORE_ACTIVATION_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "msg.h"
#include "code.h"
#include "value.h"
#include "var.h"

namespace aFuncore {
    class Inter;

    class AFUN_CORE_EXPORT Activation {
    public:
        typedef enum ActivationStatus {
            as_run = 0,
            as_end = 1,
            as_end_run = 2,
        } ActivationStatus;

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

        [[nodiscard]] inline aFuntool::FileLine getFileLine() const;
        [[nodiscard]] inline  const aFuntool::StringFilePath &getFilePath() const;

    protected:
        Activation *prev;

        VarList *varlist;

        UpMessage up;
        DownMessage down;

        aFuntool::StringFilePath path;
        aFuntool::FileLine line;

        virtual void runCodeElement(Code *code);
        virtual void runCodeBlockP(Code *code);
        virtual void runCodeBlockC(Code *code);
        virtual void runCodeBlockB(Code *code);
    };

    class AFUN_CORE_EXPORT ExeActivation : public Activation {
    public:
        explicit inline  ExeActivation(Code *code, Inter &inter_);
        ActivationStatus getCode(Code *&code) override;
        [[nodiscard]] inline  Code *getStart() const;

    private:
        Code *start;
        Code *next;
        bool first=true;
    };

    class AFUN_CORE_EXPORT TopActivation : public ExeActivation {
    public:
        explicit TopActivation(Code *code, Inter &inter_);
        ~TopActivation() override;
    };

    class AFUN_CORE_EXPORT FuncActivation : public Activation {
    public:
        explicit inline FuncActivation(Code *code, Inter &inter_);
        ~FuncActivation() override;
        ActivationStatus getCode(Code *&code) override;
        void endRun() override;

    private:
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
    };
}

#include "core-activation.inline.h"
#include "core-activation.template.h"

#endif //AFUN_CORE_ACTIVATION_H
