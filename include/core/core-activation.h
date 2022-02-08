#ifndef AFUN_CORE_ACTIVATION_H
#define AFUN_CORE_ACTIVATION_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "msg.h"
#include "code.h"
#include "object-value.h"

namespace aFuncore {
    class Inter;

    class AFUN_CORE_EXPORT Activation {
    public:
        typedef enum ActivationStatus {
            as_run = 0,
            as_end = 1,
            as_end_run = 2,
        } ActivationStatus;

        class AFUN_CORE_EXPORT VarList {
        public:
            AFUN_INLINE explicit VarList();
            virtual ~VarList();
            AFUN_INLINE VarList(VarList &&new_varlist);
            AFUN_INLINE VarList &operator=(VarList &&new_varlist) noexcept;
            VarList(const VarList &) = delete;
            VarList &operator=(const VarList &) = delete;

            void clear();
            void connect(VarList &new_varlist);
            AFUN_INLINE void push(VarSpace *varspace_);
            AFUN_INLINE size_t count();

            template <typename Callable,typename...T>
            void forEach(Callable func, T...arg);

            [[nodiscard]] virtual Var *findVar(const std::string &name);
            virtual bool defineVar(const std::string &name, Object *data);
            virtual bool defineVar(const std::string &name, Var *data);
            virtual bool setVar(const std::string &name, Object *data);
            virtual bool delVar(const std::string &name);
            [[nodiscard]] AFUN_INLINE Object *findObject(const std::string &name);

        private:
            std::list<VarSpace *> varspace;
        };

        Inter &inter;

        explicit Activation(Inter &inter_);
        virtual ~Activation();
        Activation &operator=(const Activation &)=delete;

        virtual ActivationStatus getCode(const Code::ByteCode *&code) = 0;
        virtual void runCode(const Code::ByteCode *code);
        virtual void endRun();

        [[nodiscard]] AFUN_INLINE VarList &getVarlist();
        [[nodiscard]] AFUN_INLINE UpMessage &getUpStream();
        [[nodiscard]] AFUN_INLINE DownMessage &getDownStream();

        [[nodiscard]] AFUN_INLINE aFuntool::FileLine getFileLine() const;
        [[nodiscard]] AFUN_INLINE  const aFuntool::FilePath &getFilePath() const;

    protected:
        VarList varlist;

        UpMessage up;
        DownMessage down;

        aFuntool::FilePath path;
        aFuntool::FileLine line;

        virtual void runCodeElement(const Code::ByteCode *code);
        virtual void runCodeBlockP(const Code::ByteCode *code);
        virtual void runCodeBlockC(const Code::ByteCode *code);
        virtual void runCodeBlockB(const Code::ByteCode *code);
    };

    class AFUN_CORE_EXPORT ExeActivation : public Activation {
    public:
        AFUN_INLINE ExeActivation(const Code &code, Inter &inter_);
        AFUN_INLINE ExeActivation(const Code::ByteCode *code, Inter &inter_);
        ActivationStatus getCode(const Code::ByteCode *&code) override;
        [[nodiscard]] AFUN_INLINE const Code::ByteCode *getStart() const;

    private:
        const Code::ByteCode *start;
        const Code::ByteCode *next;
        bool first=true;
    };

    class AFUN_CORE_EXPORT TopActivation : public ExeActivation {
    public:
        explicit TopActivation(const Code &code, Inter &inter_);
        ~TopActivation() override = default;
        [[nodiscard]] AFUN_INLINE const Code &getBase() const;

    private:
        const Code &base;
    };

    class AFUN_CORE_EXPORT FuncActivation : public Activation {
    public:
        AFUN_INLINE explicit FuncActivation(const Code::ByteCode *code, Inter &inter_);
        explicit FuncActivation(Function *func, Inter &inter_);
        ~FuncActivation() override;
        ActivationStatus getCode(const Code::ByteCode *&code) override;
        void endRun() override;

    private:
        enum {
            func_first = 0,  // 获取函数体前准备
            func_get_func = 1,  // 获取函数体后，开始获取参数前
            func_get_arg = 2,  // 获取参数过程
        } status = func_first;

        bool on_tail = false;
        const Code::ByteCode *call;

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
