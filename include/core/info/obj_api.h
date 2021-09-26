/*
 * 文件名: obj_api.h
 * 目标:
 * 1) 定义一些函数签名
 * 2) 定义Object的函数签名
 */

#ifndef AFUN_OBJ_API_H
#define AFUN_OBJ_API_H
#include "env.h"
#include "object.h"
#include "var.h"
#include "func.h"
#include "gc.h"

/* 定义一些函数原型 */
/* API函数 */
typedef void objectAPIFunc();  // 位于object.h (所有Object API函数指针都转换为该类型存储, 注: 具体调用类型参见下文)

/* 顶层信号处理器函数 */
typedef void TopMsgProcessFunc(af_Message *msg, bool is_gc, af_Environment *env);  // 位于env.h

/* 回调C函数 */
typedef struct CallFuncInfo CallFuncInfo;
typedef struct af_FuncBody *callFuncBody(CallFuncInfo *info, af_Environment *env);  // 位于env.h

/* 定义Object的函数签名 */
/*
 * API的第一个参数必须为: id
 * API第二个参数通常为: object
 * API第三个参数: 通常为void *data [仅与data有关的函数会直接传入该值]
 */

#define BASE_ARG char *id, af_Object *obj /* 基础参数 */

/*** Object void *data 管理 ***/
typedef size_t obj_getDataSize(BASE_ARG);  // 获取data的大小
typedef void obj_initData(BASE_ARG, void *data, af_Environment *env);  // 初始化data
typedef void obj_destructData(BASE_ARG, void *data, af_Environment *env);  // 释放data的内容 (但不释放void *data)指针

/*** Object 面向对象管理 ***/
typedef af_VarSpace *obj_getShareVarSpace(BASE_ARG);

/*** Object 函数调用 ***/
/* 获取参数计算表 */
typedef bool obj_funcGetArgCodeList(BASE_ARG, af_ArgCodeList **acl, af_Code *code, void **mark, af_Environment *env);
/* 获取函数变量空间 */
typedef bool obj_funcGetVarList(BASE_ARG, af_VarSpaceListNode **vsl, void *mark, af_Environment *env);
/* 获取参数赋值表 */
typedef bool obj_funcGetArgList(BASE_ARG, af_ArgList **al, af_ArgCodeList *acl, void *mark, af_Environment *env);
/* 获取函数信息 */
typedef bool obj_funcGetInfo(BASE_ARG, af_FuncInfo **fi, af_Code *code, void *mark, af_Environment *env);
/* 释放mask的函数 */
typedef void obj_funcFreeMask(BASE_ARG, void *mark);

/*** Object 字面量设定 ***/
typedef void obj_literalSetting(BASE_ARG, void *data, char *str, af_Environment *env);

/*** Object 函数管理 ***/
typedef bool obj_isObjFunc(BASE_ARG);  // 是否对象函数
typedef bool obj_isInfixFunc(BASE_ARG);  // 是否中缀函数

/*** Object gc管理 ***/
typedef af_GcList *obj_getGcList(BASE_ARG, void *data);  // 是否对象函数

#undef BASE_ARG
#endif //AFUN_OBJ_API_H
