#ifndef AFUN_RT_OBJECT_H
#define AFUN_RT_OBJECT_H
#include <list>
#include <mutex>
#include "aFunRuntimeExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class AFUN_RT_EXPORT Var : public aFuncore::Object {
    public:
        aFuncore::Environment &env;

        Var(Object *data_, aFuncore::Inter &inter);
        Var(Object *data_, aFuncore::Environment &env_);
        ~Var() override = default;

        [[nodiscard]] virtual Object *getData();
        virtual void setData(Object *data_);
        void linkObject(std::queue<Object *> &queue) override;

    private:
        Object *data;
    };

    class AFUN_RT_EXPORT VarSpace : public aFuncore::Object {
    public:
        typedef enum VarOperationFlat {
            vof_success = 0,  // 成功
            vof_not_var = 1,  // 变量不存在
            vof_redefine_var = 2,  // 变量重复定义
            vof_fail = 3,  // 存在其他错误
        } VarOperationFlat;

        aFuncore::Environment &env;

        explicit VarSpace(aFuncore::Inter &inter);
        explicit VarSpace(aFuncore::Environment &env_);
        ~VarSpace() override = default;

        template <typename Callable,typename...T>
        void forEach(Callable func, T...arg);

        template <typename Callable,typename...T>
        void forEachLock(Callable func, T...arg);

        [[nodiscard]] AFUN_INLINE size_t getCount();
        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual VarOperationFlat defineVar(const std::string &name, aFuncore::Object *data);
        virtual VarOperationFlat defineVar(const std::string &name, Var *data);
        virtual VarOperationFlat setVar(const std::string &name, aFuncore::Object *data);
        virtual VarOperationFlat delVar(const std::string &name);

        [[nodiscard]] AFUN_INLINE aFuncore::Object *findObject(const std::string &name);
        void linkObject(std::queue<aFuncore::Object *> &queue) override;

        AFUN_STATIC const size_t VAR_HASH_SIZE = 100;  // 环境变量哈希表大小

    private:
        std::unordered_map<std::string, Var *> var;
    };

    class AFUN_RT_EXPORT Function : public virtual aFuncore::Object {
    public:
        class AFUN_RT_EXPORT CallFunction;

        virtual CallFunction *getCallFunction(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter) = 0;
        virtual bool isInfix();
    };

    class AFUN_RT_EXPORT Function::CallFunction {
    public:
        class ArgCodeList;

        CallFunction() = default;
        virtual ~CallFunction() = default;
        CallFunction(const CallFunction &)=delete;
        CallFunction &operator=(const CallFunction &)=delete;

        virtual std::list<ArgCodeList> *getArgCodeList(aFuncore::Inter &inter, aFuncore::Activation &activation, const aFuncode::Code::ByteCode *call) = 0;
        virtual void runFunction() = 0;
    };

    class Function::CallFunction::ArgCodeList {
    public:
        const aFuncode::Code::ByteCode *code = nullptr;
        AFUN_INLINE explicit ArgCodeList(const aFuncode::Code::ByteCode *code = nullptr);
        AFUN_INLINE ~ArgCodeList();
        AFUN_INLINE aFuncore::Object *setObject(aFuncore::Object *res);
        AFUN_INLINE aFuncore::Object *getObject();
    private:
        aFuncore::Object *ret;
    };

    class AFUN_RT_EXPORT Literaler : public virtual aFuncore::Object {
    public:
        virtual void getObject(const std::string &literal, char prefix, aFuncore::Inter &inter, aFuncore::Activation &activation) = 0;
    };

    class AFUN_RT_EXPORT CallBackVar : public virtual aFuncore::Object {
    public:
        virtual bool isCallBack(aFuncore::Inter &inter, aFuncore::Activation &activation);
        virtual void callBack(aFuncore::Inter &inter, aFuncore::Activation &activation) = 0;
    };
};

#include "rt-object.inline.h"
#include "rt-object.template.h"

#endif //AFUN_RT_OBJECT_H
