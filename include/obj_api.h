#ifndef AFUN__OBJ_API_H
#define AFUN__OBJ_API_H

/* Object void *data 管理 */
typedef size_t obj_getDataSize(void);  // 获取data的大小
typedef void obj_initData(void *data);  // 初始化data
typedef void obj_freeData(void *data);  // 释放data的内容

#endif //AFUN__OBJ_API_H
