#ifndef AFUN_VAR_HPP
#define AFUN_VAR_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    static const size_t VAR_HASH_SIZE = 100;  // 环境变量哈希表大小

    typedef enum VarOperationFlat {
        vof_success = 0,  // 成功
        vof_not_var = 1,  // 变量不存在
        vof_redefine_var = 2,  // 变量重复定义
        vof_fail = 3,  // 存在其他错误
    } VarOperationFlat;

    class Var;
    class VarSpace;
    class VarList;
    class ProtectVarSpace;
}

#include "gc.hpp"
#include "value.hpp"

namespace aFuncore {
    class Var : public GcObject<class Var> {
        friend class Inter;
        Inter *inter;
        Object *data;
    public:
        Var(Object *data_, Inter *inter_);
        ~Var() override =default;

        virtual Object *getData() {return data;}
        virtual void setData(Object *data_) {data = data_;}
    };

    class VarSpace : public GcObject<class VarSpace> {
        friend class Inter;
        Inter *inter;
        struct VarCup {
            std::string name;
            Var *var;
            VarCup *next=nullptr;
        };
        size_t count;
        VarCup *var[VAR_HASH_SIZE];
    public:
        explicit VarSpace(Inter *inter_);
        ~VarSpace() override;

        virtual Var *findVar(const std::string &name);
        virtual VarOperationFlat defineVar(const std::string &name, Object *data);
        virtual VarOperationFlat defineVar(const std::string &name, Var *data);
        virtual VarOperationFlat setVar(const std::string &name, Object *data);
        virtual VarOperationFlat delVar(const std::string &name);

        Object *findObject(const std::string &name) {
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
        VarSpace *const varspace;
        VarList *next;
    public:
        explicit VarList(VarSpace *vs) : varspace {vs}, next {nullptr} {};
        void destructAll();

        virtual Var *findVar(const std::string &name);
        virtual bool defineVar(const std::string &name, Object *data);
        virtual bool defineVar(const std::string &name, Var *data);
        virtual bool setVar(const std::string &name, Object *data);
        virtual bool delVar(const std::string &name);
        Object *findObject(const std::string &name) {
            Var *var = findVar(name);
            return var ? var->getData() : nullptr;
        }

        VarList *toNext() {return next;}
        VarList *connect(VarList *varlist) {next = varlist; return this;}
        void disconnect(VarList *varlist);
    };
}

#endif //AFUN_VAR_HPP
