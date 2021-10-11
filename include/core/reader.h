#ifndef AFUN_READER_H
#define AFUN_READER_H
#include "tool.h"

#define READER_MODE_NORMAL (0)
#define READER_MODE_FINISHED (1)
#define READER_MODE_ERROR (2)

typedef size_t readerFunc(void *data, char *dest, size_t len, int *mde);
DEFINE_DLC_SYMBOL(readerFunc);

typedef void destructReaderFunc(void *data);
DEFINE_DLC_SYMBOL(destructReaderFunc);

#endif //AFUN_READER_H
