#ifndef AFUN_VAR_HPP
#define AFUN_VAR_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "core.hpp"
#include "gc.hpp"

namespace aFuncore {
    class Var : public GcObject<class Var> {
        Object *data;
    public:
        Inter *const inter;

        Var(Object *data_, Inter *inter_);
        ~Var() override =default;

        [[nodiscard]] virtual Object *getData() {return data;}
        virtual void setData(Object *data_) {data = data_;}
    };

    class VarSpace : public GcObject<class VarSpace> {
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
        Inter *const inter;
        explicit VarSpace(Inter *inter_);
        ~VarSpace() override;

        [[nodiscard]] size_t getCount() const {return count;}
        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual VarOperationFlat defineVar(const std::string &name, Object *data);
        virtual VarOperationFlat defineVar(const std::string &name, Var *data);
        virtual VarOperationFlat setVar(const std::string &name, Object *data);
        virtual VarOperationFlat delVar(const std::string &name);

        [[nodiscard]] Object *findObject(const std::string &name) {
            Var *ret = findVar(name);
            return ret ? ret->getData() : nullptr;
        }
    };

    class ProtectVarSpace : public VarSpace {
        bool is_protect;
    public:
        explicit ProtectVarSpace(Inter *inter_) : VarSpace(inter_), is_protect{false} {}

        [[nodiscard]]bool getProtect() const {return is_protect;};
        bool setProtect(bool protect) {bool ret = is_protect; is_protect = protect; return ret;}

        VarOperationFlat defineVar(const std::string &name, Object *data) override;
        VarOperationFlat defineVar(const std::string &name, Var *data) override;
        VarOperationFlat setVar(const std::string &name, Object *data) override;
        VarOperationFlat delVar(const std::string &name) override;
    };

    class VarList {
        VarList *next;
    public:
        VarSpace *const varspace;

        explicit VarList(VarSpace *vs) : varspace {vs}, next {nullptr} {};
        void destructAll();

        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual bool defineVar(const std::string &name, Object *data);
        virtual bool defineVar(const std::string &name, Var *data);
        virtual bool setVar(const std::string &name, Object *data);
        virtual bool delVar(const std::string &name);
        [[nodiscard]] Object *findObject(const std::string &name) {
            Var *var = findVar(name);
            return var ? var->getData() : nullptr;
        }

        [[nodiscard]] VarList *toNext() const {return next;}
        VarList *connect(VarList *varlist) {next = varlist; return this;}
        void disconnect(VarList *varlist);
        void disconnectNext() {next = nullptr;}
    };
}

#endif //AFUN_VAR_HPP
