#include <stdio.h>
#include "aFun.h"

/* 测试程序, 直接调用内部函数 */
typedef struct af_Reader af_Reader;

typedef size_t readerFunc(void *data, char *dest, size_t len);
DEFINE_DLC_SYMBOL(readerFunc);

typedef void destructReaderDataFunc(void *data);
DEFINE_DLC_SYMBOL(destructReaderDataFunc);

af_Reader *makeReader(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderDataFunc) destruct_func, size_t data_size);
void freeReader(af_Reader *reader);
af_Reader *initReader(af_Reader *reader);
void *getReaderData(af_Reader *reader);
char *readWord(size_t del_index, af_Reader *reader);
char getChar(af_Reader *reader);

size_t readTest(int *data, char *dest, size_t len) {
    *data = *data + 1;
    if (*data == 1) {
        memset(dest, 's', len);
        return len;
    } else if (*data == 2) {
        memset(dest, 'w', len - 5);
        return len - 5;
    }
    return 0;
}

void destructTest(const int *data) {
    printf("destructTest data = %d\n", *data);
}

int main() {
    printf("HelloWorld\n");

    {
        DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readTest, readerFunc);
        DLC_SYMBOL(destructReaderDataFunc) destruct_func = MAKE_SYMBOL(destructTest, destructReaderDataFunc);
        af_Reader *reader = makeReader(read_func, destruct_func, sizeof(int));
        *(int *) getReaderData(reader) = 0;
        initReader(reader);
        FREE_SYMBOL(read_func);
        FREE_SYMBOL(destruct_func);

        char ch;
        size_t count = 0;
        do {
            ch = getChar(reader);
            printf("%zu\t\tch = %c\n", count, ch);
            count++;
        } while (ch != NUL);

        freeReader(reader);
    }

    {
        DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readTest, readerFunc);
        DLC_SYMBOL(destructReaderDataFunc) destruct_func = MAKE_SYMBOL(destructTest, destructReaderDataFunc);
        af_Reader *reader = makeReader(read_func, destruct_func, sizeof(int));
        *(int *) getReaderData(reader) = 0;
        initReader(reader);
        FREE_SYMBOL(read_func);
        FREE_SYMBOL(destruct_func);

        char ch;
        for (size_t i = 0; i < 1000; i++) {
            ch = getChar(reader);
            printf("%zu\t\tch = %c\n", i, ch);
        }
        char *new = readWord(100, reader);  // 提取前面100个值
        free(new);

        size_t count = 0;
        do {
            ch = getChar(reader);
            printf("%zu\t\tch = %c\n", count, ch);  // 923-s 1019-w
            count++;
        } while (ch != NUL);

        freeReader(reader);
    }

    return 0;
}