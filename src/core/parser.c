﻿/*
 * 文件名: parser.c
 * 目标: __parser.h中结构体的相关函数
 */

#include "aFunCore.h"
#include "__parser.h"

static af_Lexical *makeLexical(void);
static void freeLexical(af_Lexical *lex);
static af_Syntactic *makeSyntactic(void);
static void freeSyntactic(af_Syntactic *syntactic);

af_Parser *makeParser(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size){
    af_Parser *parser = calloc(1, sizeof(af_Parser));
    parser->reader = makeReader(read_func, destruct_func, data_size);
    parser->lexical = makeLexical();
    parser->syntactic = makeSyntactic();
    return parser;
}

void freeParser(af_Parser *parser) {
    freeReader(parser->reader);
    freeLexical(parser->lexical);
    freeSyntactic(parser->syntactic);
    free(parser);
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

static size_t readFuncString(struct readerDataString *data, char *dest, size_t len, bool *read_end) {
    if (data->index == data->len)  // 读取到末尾
        return 0;

    if (data->index + len > data->len) {  // 超出长度范围
        len = data->len - data->index;
        *read_end = true;
    }
    memcpy(dest, data->str + data->index, len);
    data->index += len;
    return len;
}

static void destructFunc(struct readerDataString *data) {
    if (data->free_str)
        free(data->str);
}

af_Parser *makeParserByString(char *str, bool free_str){
    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncString, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructFunc, destructReaderFunc);
    af_Parser *parser = makeParser(read_func, destruct, sizeof(struct readerDataString));
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

static size_t readFuncFile(struct readerDataFile *data, char *dest, size_t len, bool *read_end) {
    size_t len_r =  fread(dest, sizeof(char), len, data->file);
    if (feof(data->file))
        *read_end = true;
    return len_r;
}

static void destructFile(struct readerDataFile *data) {
    if (data->file != NULL)
        fclose(data->file);
}

af_Parser *makeParserByFile(FilePath path){
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        writeErrorLog(aFunCoreLogger, log_default, "File open error: %s", file);
        return NULL;
    }

    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncFile, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructFile, destructReaderFunc);
    af_Parser *parser = makeParser(read_func, destruct, sizeof(struct readerDataString));
    ((struct readerDataFile *)parser->reader->data)->file = file;
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}

struct readerDataStdin {
    bool no_first;

    char *data;
    size_t index;
    size_t len;
};

static size_t readFuncStdin(struct readerDataStdin *data, char *dest, size_t len, bool *read_end) {
    if (data->index == data->len) {  // 读取内容
        if (data->no_first)
            printf("... ");
        else
            printf(">>> ");
        data->no_first = true;

        free(data->data);

        int ch = getc(stdin);
        if (ch == '\n' || ch == EOF) {
            /* 读取结束 */
            *read_end = true;
            return 0;
        }

        ungetc(ch, stdin);

        if (fgets_stdin(&data->data, STDIN_MAX_SIZE) == 0) {
            writeErrorLog(aFunCoreLogger, log_default, "The stdin buf too large (> %d)", STDIN_MAX_SIZE);
            *read_end = true;
            return 0;
        }

        data->index = 0;
        data->len = strlen(data->data);
    }

    if (data->index + len > data->len)  // 超出长度范围
        len = data->len - data->index;
    memcpy(dest, data->data + data->index, len);
    data->index += len;
    return len;
}

static void destructStdin(struct readerDataStdin *data) {
    free(data->data);
}

af_Parser *makeParserByStdin(){
    if (ferror(stdin))
        clearerr(stdin);

    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncStdin, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructStdin, destructReaderFunc);
    af_Parser *parser = makeParser(read_func, destruct, sizeof(struct readerDataStdin));
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}