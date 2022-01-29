#include "value.h"
#include "inter.h"

namespace aFuncore {
    Object::Object(std::string type_, Inter &inter)
            : type{std::move(type_)}, env{inter.getEnvironment()}{
        std::unique_lock<std::mutex> mutex{env.lock};
        this->addObject(env.obj);
    }

    Object::Object(std::string type_, Environment &env_)
            : type{std::move(type_)}, env{env_}{
        std::unique_lock<std::mutex> mutex{env.lock};
        this->addObject(env.obj);
    }
}