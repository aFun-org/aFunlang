#ifndef AFUN_GC
#define AFUN_GC
#include "aFunCoreExport.h"
#include "object.h"
#include "var.h"

typedef uint32_t GcCount;
typedef struct af_GcList af_GcList;
enum af_GcListType {
    glt_obj = 1,
    glt_vs,
    glt_var,
    glt_vsl,
};

/* GcList 创建与释放 */
AFUN_CORE_EXPORT af_GcList *makeGcList(enum af_GcListType type, void *data);
AFUN_CORE_EXPORT void freeAllGcList(af_GcList *gl);

/* GcList 相关操作 */
AFUN_CORE_EXPORT af_GcList *pushGcList(enum af_GcListType type, void *data, af_GcList *base);

/* gc Reference 相关操作 */
AFUN_CORE_EXPORT void gc_addObjectReference(af_Object *obj, af_Environment *base);
AFUN_CORE_EXPORT void gc_addVarReference(af_Var *obj, af_Environment *base);
AFUN_CORE_EXPORT void gc_addVarSpaceReference(af_VarSpace *obj, af_Environment *base);
AFUN_CORE_EXPORT void gc_delObjectReference(af_Object *obj, af_Environment *base);
AFUN_CORE_EXPORT void gc_delVarReference(af_Var *obj, af_Environment *base);
AFUN_CORE_EXPORT void gc_delVarSpaceReference(af_VarSpace *obj, af_Environment *base);
AFUN_CORE_EXPORT void gc_delVarListReference(af_VarList *vsl, af_Environment *base);

/* gc 操控函数 : gc的启动由解释器完全管理 */
AFUN_CORE_EXPORT af_GuardianList *gc_RunGC(af_Environment *env);
#endif //AFUN_GC
