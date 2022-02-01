#ifndef AFUN_OBJECT_VALUE_H
#define AFUN_OBJECT_VALUE_H
#include <list>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "object.h"
#include "code.h"
#include "inter.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Var : public Object {
    public:
        Environment &env;

        Var(Object *data_, Inter &inter);
        Var(Object *data_, Environment &env_);
        ~Var() override = default;

        [[nodiscard]] virtual Object *getData();
        virtual void setData(Object *data_);
        void linkObject(std::queue<Object *> &queue) override;

    private:
        Object *data;
    };

    class AFUN_CORE_EXPORT VarSpace : public Object {
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
        ~VarSpace() override = default;

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
        void linkObject(std::queue<Object *> &queue) override;

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

    class AFUN_CORE_EXPORT Function : public virtual Object {
    public:
        class AFUN_CORE_EXPORT CallFunction;

        virtual CallFunction *getCallFunction(const Code::ByteCode *code, Inter &inter) = 0;
        virtual bool isInfix();
    };

    class AFUN_CORE_EXPORT Function::CallFunction {
    public:
        struct ArgCodeList;

        CallFunction() = default;
        virtual ~CallFunction() = default;
        CallFunction(const CallFunction &)=delete;
        CallFunction &operator=(const CallFunction &)=delete;

        virtual std::list<ArgCodeList> *getArgCodeList(Inter &inter, Activation &activation, const Code::ByteCode *call) = 0;
        virtual void runFunction() = 0;
    };

    struct Function::CallFunction::ArgCodeList {
        const Code::ByteCode *code = nullptr;
        Object *ret = nullptr;
    };

    class AFUN_CORE_EXPORT Literaler : public virtual Object {
    public:
        virtual void getObject(const std::string &literal, char prefix, Inter &inter, Activation &activation) = 0;
    };

    class AFUN_CORE_EXPORT CallBackVar : public virtual Object {
    public:
        virtual bool isCallBack(Inter &inter, Activation &activation);
        virtual void callBack(Inter &inter, Activation &activation) = 0;
    };
};

#include "object-value.inline.h"
#include "object-value.template.h"

#endif //AFUN_OBJECT_VALUE_H
