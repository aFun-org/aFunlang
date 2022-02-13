#ifndef AFUN_RT_ACTIVATION_H
#define AFUN_RT_ACTIVATION_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt-object.h"

namespace aFunrt {
    class AFUN_RT_EXPORT NormalActivation : public aFuncore::Activation {
    public:
        class AFUN_RT_EXPORT VarList {
        public:
            AFUN_INLINE explicit VarList();
            virtual ~VarList();
            AFUN_INLINE VarList(VarList &&new_varlist) noexcept ;
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
            virtual bool defineVar(const std::string &name, aFuncore::Object *data);
            virtual bool defineVar(const std::string &name, Var *data);
            virtual bool setVar(const std::string &name, aFuncore::Object *data);
            virtual bool delVar(const std::string &name);
            [[nodiscard]] AFUN_INLINE aFuncore::Object *findObject(const std::string &name);

        private:
            std::list<VarSpace *> varspace;
        };

        aFuncore::Inter &inter;

        explicit NormalActivation(aFuncore::Inter &inter_);
        ~NormalActivation() override;

        void runCode(const aFuncode::Code::ByteCode *code) override;
        void endRun() override;

        [[nodiscard]] AFUN_INLINE VarList &getVarlist();
        [[nodiscard]] aFuncore::UpMessageStream &getUpStream() override;
        [[nodiscard]] aFuncore::DownMessageStream &getDownStream() override;
        [[nodiscard]] aFuntool::FileLine getFileLine() override;
        [[nodiscard]] const aFuntool::FilePath &getFilePath() override;

    protected:
        VarList varlist;

        aFuncore::UpMessageStream up;
        aFuncore::DownMessageStream down;

        aFuntool::FilePath path;
        aFuntool::FileLine line;

        virtual void runCodeElement(const aFuncode::Code::ByteCode *code);
        virtual void runCodeBlockP(const aFuncode::Code::ByteCode *code);
        virtual void runCodeBlockC(const aFuncode::Code::ByteCode *code);
        virtual void runCodeBlockB(const aFuncode::Code::ByteCode *code);
    };

    class AFUN_RT_EXPORT ExeActivation : public NormalActivation {
    public:
        AFUN_INLINE ExeActivation(const aFuncode::Code &code, aFuncore::Inter &inter_);
        AFUN_INLINE ExeActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_);
        ActivationStatus getCode(const aFuncode::Code::ByteCode *&code) override;
        [[nodiscard]] AFUN_INLINE const aFuncode::Code::ByteCode *getStart() const;

    private:
        const aFuncode::Code::ByteCode *start;
        const aFuncode::Code::ByteCode *next;
        bool first=true;
    };

    class AFUN_RT_EXPORT TopActivation : public ExeActivation {
    public:
        explicit TopActivation(const aFuncode::Code &code, aFuncore::Inter &inter_);
        ~TopActivation() override = default;
        [[nodiscard]] AFUN_INLINE const aFuncode::Code &getBase() const;

    private:
        const aFuncode::Code &base;
    };

    class AFUN_RT_EXPORT FuncActivation : public NormalActivation {
    public:
        AFUN_INLINE explicit FuncActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_);
        explicit FuncActivation(Function *func, aFuncore::Inter &inter_);
        ~FuncActivation() override;
        ActivationStatus getCode(const aFuncode::Code::ByteCode *&code) override;
        void endRun() override;

    private:
        enum {
            func_first = 0,  // 获取函数体前准备
            func_get_func = 1,  // 获取函数体后，开始获取参数前
            func_get_arg = 2,  // 获取参数过程
        } status = func_first;

        bool on_tail = false;
        const aFuncode::Code::ByteCode *call;

        Function *func = nullptr;
        Function::CallFunction *call_func = nullptr;

        std::list<Function::CallFunction::ArgCodeList> *acl = nullptr;
        std::list<Function::CallFunction::ArgCodeList>::iterator acl_begin;
        std::list<Function::CallFunction::ArgCodeList>::iterator acl_end;
    };
}

#include "rt-activation.inline.h"
#include "rt-activation.template.h"

#endif //AFUN_RT_ACTIVATION_H
