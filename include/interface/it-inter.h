#ifndef AFUN_IT_INTER_H
#define AFUN_IT_INTER_H
#include "aFunlangExport.h"
#include "aFuncore.h"

namespace aFunit {
    class AFUN_LANG_EXPORT aFunEnvironment : public aFuncore::Environment {
    public:
        explicit aFunEnvironment(int argc = 0, char **argv = nullptr);
    };
}


#endif //AFUN_IT_INTER_H
