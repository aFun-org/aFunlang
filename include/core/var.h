#ifndef AFUN_VAR_H
#define AFUN_VAR_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"
#include "gc.h"
#include <list>

namespace aFuncore {
    class AFUN_CORE_EXPORT Var : public GcObject<class Var> {
        Object *data;
    public:
        Inter &inter;

        Var(Object *data_, Inter &inter_);
        ~Var() override = default;

        [[nodiscard]] inline virtual Object *getData();
        virtual void inline setData(Object *data_);
    };

    class AFUN_CORE_EXPORT VarSpace : public GcObject<class VarSpace> {
    public:
        static const size_t VAR_HASH_SIZE = 100;  // 环境变量哈希表大小
    private:
        struct VarCup {
            std::string name;
            Var *var;
            VarCup *next=nullptr;
        };
        size_t count;
        VarCup *var[VAR_HASH_SIZE];
    public:
        Inter &inter;

        explicit VarSpace(Inter &inter_);
        ~VarSpace() override;

        template <typename Callable,typename...T>
        void forEach(Callable func, T...arg);

        [[nodiscard]] inline size_t getCount() const;
        [[nodiscard]] inline virtual Var *findVar(const std::string &name);
        virtual VarOperationFlat defineVar(const std::string &name, Object *data);
        virtual VarOperationFlat defineVar(const std::string &name, Var *data);
        virtual VarOperationFlat setVar(const std::string &name, Object *data);
        virtual VarOperationFlat delVar(const std::string &name);

        [[nodiscard]] Object *findObject(const std::string &name);
    };

    class AFUN_CORE_EXPORT ProtectVarSpace : public VarSpace {
        bool is_protect;
    public:
        explicit inline ProtectVarSpace(Inter &inter_);

        [[nodiscard]] inline bool getProtect() const;
        inline bool setProtect(bool protect);

        VarOperationFlat defineVar(const std::string &name, Object *data) override;
        VarOperationFlat defineVar(const std::string &name, Var *data) override;
        VarOperationFlat setVar(const std::string &name, Object *data) override;
        VarOperationFlat delVar(const std::string &name) override;
    };

    class AFUN_CORE_EXPORT VarList {
        std::list<VarSpace *> varspace;
    public:
        explicit inline VarList() = default;
        explicit VarList(VarList *varlist);
        explicit VarList(VarSpace *varspace);
        ~VarList() = default;
        VarList(const VarList &) = delete;
        VarList &operator=(const VarList &) = delete;

        void connect(VarList *varlist);
        inline void push(VarSpace *varspace_);

        template <typename Callable,typename...T>
        void forEach(Callable func, T...arg);

        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual bool defineVar(const std::string &name, Object *data);
        virtual bool defineVar(const std::string &name, Var *data);
        virtual bool setVar(const std::string &name, Object *data);
        virtual bool delVar(const std::string &name);
        [[nodiscard]] inline Object *findObject(const std::string &name);
    };
}

#include "var.inline.h"
#include "var.template.h"

#endif //AFUN_VAR_H
