#ifndef AFUN_VARLIST_H
#define AFUN_VARLIST_H
#include <list>
#include <unordered_map>
#include <mutex>
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "object-value.h"
#include "inter.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT VarList {
    public:
        explicit inline VarList() = default;
        explicit VarList(VarList *varlist);
        explicit VarList(VarSpace *varspace);
        ~VarList() = default;
        VarList(const VarList &) = delete;
        VarList &operator=(const VarList &) = delete;

        void connect(VarList *varlist);
        inline void push(VarSpace *varspace_);

        template <typename Callable,typename...T>
        void forEach(Callable func, T...arg);

        template <typename Callable,typename...T>
        void forEachLock(Callable func, T...arg);

        [[nodiscard]] virtual Var *findVar(const std::string &name);
        virtual bool defineVar(const std::string &name, Object *data);
        virtual bool defineVar(const std::string &name, Var *data);
        virtual bool setVar(const std::string &name, Object *data);
        virtual bool delVar(const std::string &name);
        [[nodiscard]] inline Object *findObject(const std::string &name);

        inline void GcLinkObject(std::queue<Object *> &queue);  /* 虽然不是GcObject, 但是也设定改函数便于将其包含的varspace快速压入queue中 */

    protected:
        std::mutex lock;

    private:
        std::list<VarSpace *> varspace;
    };
}

#include "varlist.inline.h"
#include "varlist.template.h"

#endif //AFUN_VARLIST_H
