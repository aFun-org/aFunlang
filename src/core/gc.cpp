#include "gc.h"
using namespace aFuncore;
using namespace aFuntool;

size_t GcList::add(GcObjectBase *obj){
    queue.push(obj);
    return queue.size();
}


GcObjectBase *GcList::pop(){
    if (queue.empty())
        return nullptr;

    GcObjectBase *ret = queue.front();
    queue.pop();
    return ret;
}
