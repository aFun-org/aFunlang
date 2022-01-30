#include "gc.h"
#include "inter.h"

namespace aFuncore {
    void GcObjectBase::destructAll(std::list<GcObjectBase *>& list, Inter &gc_inter) {
        for (auto obj : list) {
            delete obj;
        }
        list.clear();
    }

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
}