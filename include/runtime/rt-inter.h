#ifndef AFUN_RT_INTER_H
#define AFUN_RT_INTER_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFunrt {
    class aFunEnvironment : public aFuncore::Environment {
    public:
        explicit aFunEnvironment(int argc = 0, char **argv = nullptr);
    };
}


#endif //AFUN_RT_INTER_H
