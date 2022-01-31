#include "value.h"
#include "inter.h"
#include "init.h"

namespace aFuncore {
    Object::Object(std::string type_, Inter &inter)
            : type{std::move(type_)}, env{inter.getEnvironment()}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }

    Object::Object(std::string type_, Environment &env_)
            : type{std::move(type_)}, env{env_}{
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.push_back(this);
    }

    Object::~Object() {
        if (getReference() != 0)
            warningLog(aFunCoreLogger, "Object %p destruct reference: %d", this, getReference());
        std::unique_lock<std::mutex> mutex{env.lock};
        env.gc.remove(this);
    }
}