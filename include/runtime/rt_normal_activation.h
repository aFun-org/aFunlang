#ifndef AFUN_RT_NORMAL_ACTIVATION_H
#define AFUN_RT_NORMAL_ACTIVATION_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt_func_object.h"
#include "rt_varspace_object.h"
#include "rt_var_object.h"

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
}

#include "rt_normal_activation.inline.h"
#include "rt_normal_activation.template.h"

#endif //AFUN_RT_NORMAL_ACTIVATION_H
