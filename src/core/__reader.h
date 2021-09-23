#ifndef AFUN_READER_H_
#define AFUN_READER_H_
#include <stdio.h>
#include "aFunCoreExport.h"  // reader.h 不包含 aFunCoreExport.h
#include "tool.h"
#include "reader.h"

#define DEFAULT_BUF_SIZE (1024)
#define NEW_BUF_SIZE (512)

typedef struct af_Reader af_Reader;

typedef size_t readerFunc(void *data, char *dest, size_t len);
NEW_DLC_SYMBOL(readerFunc, readerFunc);

typedef void destructReaderFunc(void *data);
NEW_DLC_SYMBOL(destructReaderFunc, destructReaderFunc);

struct af_Reader {
    DLC_SYMBOL(readerFunc) read_func;
    DLC_SYMBOL(destructReaderFunc) destruct;
    void *data;
    size_t data_size;

    char *buf;
    char *buf_end;  // buf的尾部位置[NUL的位置]
    size_t buf_size;  // buf的长度-1
    char *read;
    bool read_end;
    FileLine line;

    bool init;  // 是否初始化
};

/* Reader 创建与释放 */
AFUN_CORE_NO_EXPORT af_Reader *makeReader(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size);
AFUN_CORE_NO_EXPORT void freeReader(af_Reader *reader);

/* Reader 初始化函数 */
AFUN_CORE_NO_EXPORT af_Reader *initReader(af_Reader *reader);

/* Reader 操作哈桑 */
AFUN_CORE_NO_EXPORT char *readWord(size_t del_index, af_Reader *reader);
AFUN_CORE_NO_EXPORT char getChar(af_Reader *reader);
AFUN_CORE_NO_EXPORT void *getReaderData(af_Reader *reader);

#endif //AFUN_READER_H_
