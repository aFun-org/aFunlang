#ifndef AFUN__OBJECT_H_PUBLIC
#define AFUN__OBJECT_H_PUBLIC

typedef struct af_Object af_Object;
typedef struct af_Inherit af_Inherit;

#include "env.h"
#include "tool.h"

af_Object *makeObject(char *id, size_t data_size, bool inherit_api, bool allow_iherit,
                      af_Inherit *inherit, af_Environment *env);
void freeObject(af_Object *obj);

af_Inherit *makeIherit(af_Object *obj);
af_Inherit *freeIherit(af_Inherit *ih);
void freeAllIherit(af_Inherit *ih);

void *findObjectAPI(char *api_name, af_Object *obj);
int addAPIToObject(DlcHandle *dlc, char *func_name, char *api_name,
                   af_Object *obj);
#endif //AFUN__OBJECT_H_PUBLIC
