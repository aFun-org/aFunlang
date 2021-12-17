#include "activation.hpp"

using namespace aFuncore;
using namespace aFuntool;

Activation::Activation(Inter *inter_, Activation *prev_)
        : inter{inter_}, prev{prev_}, old_varlist{prev_->varlist},
          varlist{}, line{0}{
    up = new UpMessage(prev ? prev->up : nullptr);
    down = new DownMessage();
}

Activation::~Activation(){
    varlist->disconnect(old_varlist);
    if (prev)
        down->joinMsg(prev->down);
    delete up;
    delete down;
}

Code *TopActivation::getCode(){
    Code *ret = next;
    next = ret->toNext();
    return ret;
}
