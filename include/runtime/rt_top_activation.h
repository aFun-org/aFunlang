#ifndef AFUN_RT_TOP_ACTIVATION_H
#define AFUN_RT_TOP_ACTIVATION_H
#include "aFunRuntimeExport.h"
#include "aFuncore.h"
#include "rt_func_object.h"
#include "rt_exe_activation.h"

namespace aFunrt {
    class AFUN_RT_EXPORT TopActivation : public ExeActivation {
    public:
        explicit TopActivation(const aFuncode::Code &code, aFuncore::Inter &inter_);
        ~TopActivation() override = default;
        [[nodiscard]] AFUN_INLINE const aFuncode::Code &getBase() const;

    private:
        const aFuncode::Code &base;
    };
}

#include "rt_top_activation.inline.h"
#endif //AFUN_RT_TOP_ACTIVATION_H
