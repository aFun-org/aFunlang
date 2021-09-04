#ifndef AFUN__READER_H
#define AFUN__READER_H
#include <stdio.h>
#include "macro.h"
#include "tool.h"

#define DEFAULT_BUF_SIZE (1024)
#define NEW_BUF_SIZE (512)

typedef struct af_Reader af_Reader;

typedef size_t readerFunc(void *data, char *dest, size_t len);
NEW_DLC_SYMBOL(readerFunc, readerFunc);

typedef void destructReaderDataFunc(void *data);
NEW_DLC_SYMBOL(destructReaderDataFunc, destructReaderDataFunc);

struct af_Reader {
    DLC_SYMBOL(readerFunc) read_func;
    DLC_SYMBOL(destructReaderDataFunc) destruct;
    void *data;
    size_t data_size;

    char *buf;
    char *buf_end;  // buf的尾部位置[NUL的位置]
    size_t buf_size;  // buf的长度-1
    char *read;
    bool read_end;
};

/* Reader 创建与释放 */
af_Reader *makeReader(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderDataFunc) destruct_func, size_t data_size);
void freeReader(af_Reader *reader);

/* Reader 初始化函数 */
af_Reader *initReader(af_Reader *reader);

/* Reader 操作哈桑 */
char *readWord(size_t del_index, af_Reader *reader);
char getChar(af_Reader *reader);
void *getReaderData(af_Reader *reader);

#endif //AFUN__READER_H
