#ifndef AFUN_RT_FUNC_ACTIVATION_H
#define AFUN_RT_FUNC_ACTIVATION_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt_func_object.h"
#include "rt_normal_activation.h"

namespace aFunrt {
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

#include "rt_func_activation.inline.h"
#endif //AFUN_RT_FUNC_ACTIVATION_H
