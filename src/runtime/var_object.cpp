#include "rt_var_object.h"

namespace aFunrt {
    Var::Var(Object *data_, aFuncore::Inter &inter) : Object("Var", inter), env{inter.getEnvironment()}, data{data_}{

    }

    Var::Var(Object *data_, aFuncore::Environment &env_) : Object("Var", env_), env{env_}, data{data_}{

    }

    void Var::linkObject(std::queue<aFuncore::Object *> &queue) {
        queue.push(getData());
    }

    aFuncore::Object *Var::getData() {
        std::unique_lock<std::mutex> mutex{lock};
        return data;
    }

    void Var::setData(Object *data_) {
        std::unique_lock<std::mutex> mutex{lock};
        data = data_;
    }
}