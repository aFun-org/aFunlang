#include "value.hpp"
#include "inter.hpp"
#include "__gc.hpp"

using namespace aFuncore;
using namespace aFuntool;

aFuncore::Object::Object(const std::string &type_, Inter *inter_)
        : type{type_}, inter{inter_->base} {
    this->addObject(inter->getGcRecord()->obj);
}
