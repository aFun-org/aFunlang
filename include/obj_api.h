/*
 * 文件名: obj_api.h
 * 目标:
 * 1) 定义一些函数签名
 * 2) 定义Object的函数签名
 */

#ifndef AFUN__OBJ_API_H
#define AFUN__OBJ_API_H
#include "env.h"
#include "object.h"
#include "var.h"
#include "func.h"

/* 定义一些函数原型 */
/* API函数 */
typedef void objectAPIFunc();  // 位于object.h (所有Object API函数指针都转换为该类型存储, 注: 具体调用类型参见下文)

/* 顶层信号处理器函数 */
typedef void TopMsgProcessFunc(af_Message *msg, af_Environment *env);  // 位于env.h

/* 回调C函数 */
typedef void callFuncBody(void *mark, af_Environment *env);  // 位于env.h

/* 定义Object的函数签名 */
/* Object void *data 管理 */
typedef size_t obj_getDataSize(void);  // 获取data的大小
typedef void obj_initData(void *data);  // 初始化data
typedef void obj_freeData(void *data);  // 释放data的内容 (但不释放void *data)指针

/* Object 函数调用 */
typedef bool obj_funcGetArgCodeList(ArgCodeList **acl, af_Object *obj, af_Code *code, void **mark, af_Environment *env);  // 获取参数计算表
typedef bool obj_funcGetVarList(af_VarSpaceListNode **vsl, af_Object *obj, void *mark, af_Environment *env);  // 获取函数变量空间
typedef bool obj_funcGetArgList(ArgList **al, af_Object *obj, ArgCodeList *acl, void *mark, af_Environment *env);  // 获取参数赋值表
typedef bool obj_funcGetInfo(af_FuncInfo **fi, af_Object *obj, af_Code *code, void *mark, af_Environment *env);  // 获取函数信息
typedef void obj_funcFreeMask(void *mark);  // 释放mask的函数

#endif //AFUN__OBJ_API_H
