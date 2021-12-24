#ifndef AFUN_VALUE_HPP
#define AFUN_VALUE_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

namespace aFuncore {
    class Object;
};

#include "gc.hpp"
#include "inter.hpp"

namespace aFuncore {
    class Object : public GcObject<class Object> {
    public:
        Inter *const inter;
        const std::string type;  // 标识 Object 的字符串

        AFUN_CORE_EXPORT explicit Object(const std::string &type_, Inter *inter_);
        AFUN_CORE_EXPORT ~Object() override =default;
    };
};

#endif //AFUN_VALUE_HPP
