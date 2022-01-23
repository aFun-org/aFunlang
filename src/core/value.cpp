#include "value.h"
#include "inter.h"

namespace aFuncore {
    Object::Object(std::string type_, Inter &inter)
            : type{std::move(type_)}, env{inter.getEnvironment()}{
        this->addObject(env.obj);
    }

    Object::Object(std::string type_, Environment &env_)
            : type{std::move(type_)}, env{env_}{
        this->addObject(env.obj);
    }
}