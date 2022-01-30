﻿#ifndef AFUN_VAR_H
#define AFUN_VAR_H
#include <list>
#include <unordered_map>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "gc.h"
#include "inter.h"

namespace aFuncore {
    class Object;

    class AFUN_CORE_EXPORT Var : public GcObjectBase {
    public:
        Environment &env;

        Var(Object *data_, Inter &inter);
        Var(Object *data_, Environment &env_);
        ~Var() override;

        [[nodiscard]] inline virtual Object *getData();
        virtual void inline setData(Object *data_);

    private:
        Object *data;
    };

    class AFUN_CORE_EXPORT VarSpace : public GcObjectBase {
    public:
        typedef enum VarOperationFlat {
            vof_success = 0,  // 成功
            vof_not_var = 1,  // 变量不存在
            vof_redefine_var = 2,  // 变量重复定义
            vof_fail = 3,  // 存在其他错误
        } VarOperationFlat;

        Environment &env;

        explicit VarSpace(Inter &inter);
        explicit VarSpace(Environment &env_);
        ~VarSpace() override;

        template <typename Callable,typename...T>
        void forEach(Callable func, T...arg);

        template <typename Callable,typename...T>
        void forEachLock(Callable func, T...arg);

        [[nodiscard]] inline size_t getCount();
        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual VarOperationFlat defineVar(const std::string &name, Object *data);
        virtual VarOperationFlat defineVar(const std::string &name, Var *data);
        virtual VarOperationFlat setVar(const std::string &name, Object *data);
        virtual VarOperationFlat delVar(const std::string &name);

        [[nodiscard]] Object *findObject(const std::string &name);

        static const size_t VAR_HASH_SIZE = 100;  // 环境变量哈希表大小

    private:
        std::unordered_map<std::string, Var *> var;
    };

    class AFUN_CORE_EXPORT ProtectVarSpace : public VarSpace {
    public:
        explicit inline ProtectVarSpace(Inter &inter);
        explicit inline ProtectVarSpace(Environment &env_);

        [[nodiscard]] inline bool getProtect() const;
        inline bool setProtect(bool protect);

        VarOperationFlat defineVar(const std::string &name, Object *data) override;
        VarOperationFlat defineVar(const std::string &name, Var *data) override;
        VarOperationFlat setVar(const std::string &name, Object *data) override;
        VarOperationFlat delVar(const std::string &name) override;

    private:
        bool is_protect;
    };

    class AFUN_CORE_EXPORT VarList {
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

        template <typename Callable,typename...T>
        void forEachLock(Callable func, T...arg);

        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual bool defineVar(const std::string &name, Object *data);
        virtual bool defineVar(const std::string &name, Var *data);
        virtual bool setVar(const std::string &name, Object *data);
        virtual bool delVar(const std::string &name);
        [[nodiscard]] inline Object *findObject(const std::string &name);

    protected:
        std::mutex lock;

    private:
        std::list<VarSpace *> varspace;
    };
}

#include "var.inline.h"
#include "var.template.h"

#endif //AFUN_VAR_H
