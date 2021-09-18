/*
 * 文件名: parser.c
 * 目标: __parser.h中结构体的相关函数
 */

#include "aFunCore.h"
#include "__parser.h"
#include <errno.h>

static af_Lexical *makeLexical(void);
static void freeLexical(af_Lexical *lex);
static af_Syntactic *makeSyntactic(void);
static void freeSyntactic(af_Syntactic *syntactic);

af_Parser *makeParser(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size,
                      FILE *error) {
    af_Parser *parser = calloc(1, sizeof(af_Parser));
    parser->reader = makeReader(read_func, destruct_func, data_size);
    parser->lexical = makeLexical();
    parser->syntactic = makeSyntactic();
    parser->error = error;
    return parser;
}

void freeParser(af_Parser *parser) {
    freeReader(parser->reader);
    freeLexical(parser->lexical);
    freeSyntactic(parser->syntactic);
    free(parser);
}

void *getParserData(af_Parser *parser) {
    return getReaderData(parser->reader);
}

void initParser(af_Parser *parser) {
    initReader(parser->reader);
}

static af_Lexical *makeLexical(void) {
    af_Lexical *lex = calloc(1, sizeof(af_Lexical));
    lex->status = lex_begin;
    return lex;
}

static void freeLexical(af_Lexical *lex) {
    free(lex);
}

static af_Syntactic *makeSyntactic(void) {
    af_Syntactic *syntactic = calloc(1, sizeof(af_Syntactic));
    return syntactic;
}

static void freeSyntactic(af_Syntactic *syntactic) {
    free(syntactic->text);
    free(syntactic);
}

/* makeParser函数封装 */
struct readerDataString {
    char *str;
    bool free_str;
    size_t index;
    size_t len;
};

static size_t readFuncString(struct readerDataString *data, char *dest, size_t len) {
    if (data->index == data->len)  // 读取到末尾
        return 0;

    if (data->index + len > data->len)  // 超出长度范围
        len = data->len - data->index;
    memcpy(dest, data->str + data->index, len);
    data->index += len;
    return len;
}

static void destructFunc(struct readerDataString *data) {
    if (data->free_str)
        free(data->str);
}

af_Parser *makeParserByString(char *str, bool free_str, FILE *error) {
    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncString, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructFunc, destructReaderFunc);
    af_Parser *parser = makeParser(read_func, destruct, sizeof(struct readerDataString), error);
    ((struct readerDataString *)parser->reader->data)->str = str;
    ((struct readerDataString *)parser->reader->data)->free_str = free_str;
    ((struct readerDataString *)parser->reader->data)->len = strlen(str);
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}

struct readerDataFile {
    FILE *file;
};

static size_t readFuncFile(struct readerDataFile *data, char *dest, size_t len) {
    return fread(dest, sizeof(char), len, data->file);
}

static void destructFile(struct readerDataFile *data) {
    if (data->file != NULL)
        fclose(data->file);
}

af_Parser *makeParserByFile(FilePath path, FILE *error) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        if (error != NULL)
            fprintf(error, "File open error: %s\n", strerror(errno));
        return NULL;
    }

    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncFile, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructFile, destructReaderFunc);
    af_Parser *parser = makeParser(read_func, destruct, sizeof(struct readerDataString), error);
    ((struct readerDataFile *)parser->reader->data)->file = file;
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}