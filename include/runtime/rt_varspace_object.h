#ifndef AFUN_VARSPACE_OBJECT_H
#define AFUN_VARSPACE_OBJECT_H
#include <list>
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt_var_object.h"

namespace aFunrt {
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
}

#include "rt_varspace_object.inline.h"
#include "rt_varspace_object.template.h"

#endif //AFUN_VARSPACE_OBJECT_H
