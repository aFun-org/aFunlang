﻿#ifndef AFUN__OBJECT_H_PUBLIC
#define AFUN__OBJECT_H_PUBLIC
#include "tool.h"

typedef struct af_Object af_Object;
typedef struct af_Inherit af_Inherit;

typedef void pValueAPI();
NEW_DLC_SYMBOL(pValueAPI, pAPIFUNC);  // TODO-szh 移动到_object.h

#include "env.h"
#include "tool.h"

af_Object *makeObject(char *id, size_t data_size, bool inherit_api, bool allow_iherit, af_Object *belong,
                      af_Inherit *iherit, af_Environment *env);
void freeObject(af_Object *obj);
af_Object *getBelongObject(af_Object *object, af_Environment *env);

af_Inherit *makeIherit(af_Object *obj);
af_Inherit *freeIherit(af_Inherit *ih);
void freeAllIherit(af_Inherit *ih);

void *findObjectAPI(char *api_name, af_Object *obj);
int addAPIToObject(DLC_SYMBOL(pAPIFUNC) func, char *api_name,
                   af_Object *obj);
#endif //AFUN__OBJECT_H_PUBLIC
