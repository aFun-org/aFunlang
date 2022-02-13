#ifndef AFUN_RT_EXE_ACTIVATION_H
#define AFUN_RT_EXE_ACTIVATION_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt_func_object.h"
#include "rt_normal_activation.h"

namespace aFunrt {
    class AFUN_RT_EXPORT ExeActivation : public NormalActivation {
    public:
        AFUN_INLINE ExeActivation(const aFuncode::Code &code, aFuncore::Inter &inter_);
        AFUN_INLINE ExeActivation(const aFuncode::Code::ByteCode *code, aFuncore::Inter &inter_);
        ActivationStatus getCode(const aFuncode::Code::ByteCode *&code) override;
        [[nodiscard]] AFUN_INLINE const aFuncode::Code::ByteCode *getStart() const;

    private:
        const aFuncode::Code::ByteCode *start;
        const aFuncode::Code::ByteCode *next;
        bool first=true;
    };
}

#include "rt_exe_activation.inline.h"
#endif //AFUN_RT_EXE_ACTIVATION_H
