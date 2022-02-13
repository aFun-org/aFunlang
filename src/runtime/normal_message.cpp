#include "rt_normal_message.h"

namespace aFunrt {
    NormalMessage::NormalMessage(aFuncore::Object *obj_) : obj {obj_} {
        obj->addReference();
    }

    NormalMessage::~NormalMessage(){
        if (obj != nullptr) {
            obj->delReference();
            obj = nullptr;
        }
    }

    void NormalMessage::topProgress(aFuncore::Inter &inter, aFuncore::Activation &){
        inter.getOutMessageStream().pushMessage("NORMAL", new NormalMessage(std::move(*this)));
    }
}
