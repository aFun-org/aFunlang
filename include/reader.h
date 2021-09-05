#ifndef AFUN_READER_H
#define AFUN_READER_H
#include "macro.h"

typedef size_t readerFunc(void *data, char *dest, size_t len);
DEFINE_DLC_SYMBOL(readerFunc);

typedef void destructReaderFunc(void *data);
DEFINE_DLC_SYMBOL(destructReaderFunc);

#endif //AFUN_READER_H
