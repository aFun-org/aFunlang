﻿#ifndef AFUN_VAR_HPP
#define AFUN_VAR_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"
#include "core.hpp"
#include "gc.hpp"
#include <list>

namespace aFuncore {
    AFUN_CORE_EXPORT class Var : public GcObject<class Var> {
        Object *data;
    public:
        Inter *const inter;

        Var(Object *data_, Inter *inter_);
        ~Var() override =default;

        [[nodiscard]] virtual Object *getData() {return data;}
        virtual void setData(Object *data_) {data = data_;}
    };

    AFUN_CORE_EXPORT class VarSpace : public GcObject<class VarSpace> {
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

    AFUN_CORE_EXPORT class ProtectVarSpace : public VarSpace {
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

    AFUN_CORE_EXPORT class VarList {
        std::list<VarSpace *> varspace;
    public:
        explicit VarList() = default;
        explicit VarList(VarList *varlist);
        explicit VarList(VarSpace *varspace) {this->varspace.push_front(varspace);}
        ~VarList() = default;
        VarList(const VarList &)=delete;
        VarList &operator=(const VarList &)=delete;

        void connect(VarList *varlist);
        void push(VarSpace *varspace_) {varspace.push_front(varspace_);}

        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual bool defineVar(const std::string &name, Object *data);
        virtual bool defineVar(const std::string &name, Var *data);
        virtual bool setVar(const std::string &name, Object *data);
        virtual bool delVar(const std::string &name);
        [[nodiscard]] Object *findObject(const std::string &name) {
            Var *var = findVar(name);
            return var ? var->getData() : nullptr;
        }
    };
}

#endif //AFUN_VAR_HPP
