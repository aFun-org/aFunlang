#ifndef AFUN_VAR_OBJECT_H
#define AFUN_VAR_OBJECT_H
#include <list>
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
}

#endif //AFUN_VAR_OBJECT_H
