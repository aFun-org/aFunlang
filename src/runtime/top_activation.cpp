#include "rt_top_activation.h"
#include "rt_normal_message.h"

namespace aFunrt {
    TopActivation::TopActivation(const aFuncode::Code &code, aFuncore::Inter &inter_) : ExeActivation(code, inter_), base{code} {

    }
}