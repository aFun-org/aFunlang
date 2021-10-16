/*
 * 文件名: parser.c
 * 目标: __parser.h中结构体的相关函数
 */

#include "aFunCore.h"
#include "tool.h"
#include "__parser.h"

static af_Lexical *makeLexical(void);
static void freeLexical(af_Lexical *lex);
static af_Syntactic *makeSyntactic(void);
static void freeSyntactic(af_Syntactic *syntactic);

af_Parser *makeParser(FilePath file, DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func,
                      size_t data_size){
    af_Parser *parser = calloc(1, sizeof(af_Parser));
    parser->reader = makeReader(1, file, read_func, destruct_func, data_size);
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
#define printReaderError(info, parser, stream) do { \
    writeErrorLog(aFunCoreLogger, "[Reader] %s:%d %s", (parser)->reader->file, (parser)->reader->line, (info ## Log)); \
    printf_std##stream(0, "[%s] %s:%d : %s\n", HT_aFunGetText(reader_n, "Reader"), (parser)->reader->file, \
                  (parser)->reader->line, info ## Console); \
    (parser)->is_error = true; /* 错误标记在Parser而非Lexical中, Lexical的异常表示lexical停止运行 */ \
} while(0)

struct readerDataString {
    af_Parser *parser;
    char *str;
    bool free_str;
    size_t index;
    size_t len;
};

static size_t readFuncString(struct readerDataString *data, char *dest, size_t len, int *mode) {
    if (data->index == data->len)  // 读取到末尾
        return 0;

    if (data->index + len > data->len) {  // 超出长度范围
        len = data->len - data->index;
        *mode = READER_MODE_FINISHED;
    }
    memcpy(dest, data->str + data->index, len);
    data->index += len;
    return len;
}

static void destructFunc(struct readerDataString *data) {
    if (data->free_str)
        free(data->str);
}

static void initStringReader(af_Parser *parser, char *str, bool free_str, struct readerDataString *data) {
    data->str = str;
    data->free_str = free_str;
    data->len = strlen(str);
    data->parser = parser;
}

af_Parser *makeParserByString(FilePath name, char *str, bool free_str){
    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncString, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructFunc, destructReaderFunc);
    af_Parser *parser = makeParser(name, read_func, destruct, sizeof(struct readerDataString));
    initStringReader(parser, str, free_str, parser->reader->data);
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}

struct readerDataFile {
    af_Parser *parser;
    FILE *file;
    bool no_first;
};

static size_t readFuncFile(struct readerDataFile *data, char *dest, size_t len, int *mode) {
    if (!data->no_first) {
        data->no_first = true;
        char ch;
        if (fread(&ch, sizeof(char), 1, data->file) != 1) {
            *mode = READER_MODE_FINISHED;
            return 0;
        }

        if (ch == (char)0xEF) {
            /* 处理BOM编码 */
            char ch_[2];
            if (fread(ch_, sizeof(char), 2, data->file) != 2 || ch_[0] != (char)0xBB || ch_[1] != (char)0xBF) {
                printReaderError(BOMError, data->parser, err);
                *mode = READER_MODE_ERROR;
                return 0;
            }
            writeTrackLog(aFunCoreLogger, "Parser utf-8 with BOM");
        } else {
            ungetc(ch, data->file);
            writeTrackLog(aFunCoreLogger, "Parser utf-8 without BOM");
        }
    }

    size_t len_r =  fread(dest, sizeof(char), len, data->file);
    if (CLEAR_FERROR(data->file)) {  // ferror在feof前执行
        *mode = READER_MODE_ERROR;
        printReaderError(FileIOError, data->parser, err);
    } else if (feof(data->file))
        *mode = READER_MODE_FINISHED;
    return len_r;
}

static void destructFile(struct readerDataFile *data) {
    if (data->file != NULL)
        fileClose(data->file);
}

static void initFileReader(af_Parser *parser, FILE *file, struct readerDataFile *data) {
    data->file = file;
    data->parser = parser;
}

af_Parser *makeParserByFile(FilePath path){
    FILE *file = fileOpen(path, "rb");
    if (file == NULL) {
        writeErrorLog(aFunCoreLogger, "File open error: %s", path);
        return NULL;
    } else
        writeTrackLog(aFunCoreLogger, "File: %s", path);

    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncFile, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructFile, destructReaderFunc);
    af_Parser *parser = makeParser(path, read_func, destruct, sizeof(struct readerDataString));
    initFileReader(parser, file, parser->reader->data);
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}

struct readerDataStdin {
    af_Parser *parser;
    bool no_first;

    void *sig_int;
    void *sig_term;

    char *data;
    size_t index;
    size_t len;
};

static volatile sig_atomic_t stdin_interrupt = 0;

static void stdinSignalFunc(int signum) {
    stdin_interrupt = 1;
}

static void setStdinSignalFunc(struct readerDataStdin *data) {
    data->sig_int = signal(SIGINT, stdinSignalFunc);
    data->sig_term = signal(SIGTERM, stdinSignalFunc);
}

static void resetStdinSignalFunc(void) {
    stdin_interrupt = 0;
    signal(SIGINT, stdinSignalFunc);
    signal(SIGTERM, stdinSignalFunc);
}

static bool getStdinSignalFunc(void) {
    bool re = stdin_interrupt == 1;
    stdin_interrupt = 0;
    resetStdinSignalFunc();
    return re;
}

static size_t readFuncStdin(struct readerDataStdin *data, char *dest, size_t len, int *mode) {
    if (data->index == data->len) {  // 读取内容
        if (CLEAR_STDIN()) {
            printReaderError(StdinError, data->parser, err);
            *mode = READER_MODE_ERROR;
            return 0;
        }

        if (data->no_first)
            fputs("\r.... ", stdout);
        else {
            fclear_stdin();
            fputs("\r>>>> ", stdout);
        }
        fflush(stdout);
        data->no_first = true;
        free(data->data);

        /* 在Linux平台, 只用当数据写入stdin缓冲行时checkStdin才true */
        /* 在Windows平台则是根据读取的最后一个字符是否为\n或者是否有按键按下来确定缓冲区是否有内容 */
        while (!checkStdin()) {  // 无内容则一直循环等到
            if (getStdinSignalFunc()) {  // 设置了中断函数, 并且该函数返回0
                writeErrorLog(aFunCoreLogger, "Interrupt");
                printf_stdout(0, "%s\n", HT_aFunGetText(Interrupt_n, "Interrupt"));
                data->parser->is_error = true;
                *mode = READER_MODE_ERROR;
                return 0;
            }
        }

        int ch = fgetc_stdin();
        if (ch == '\n' || ch == EOF) {
            /* 读取结束 */
            *mode = READER_MODE_FINISHED;
            return 0;
        }

        fungetc_stdin(ch);

        /* 读取内容的长度不得少于STDIN_MAX_SZIE, 否则可能导致编码转换错误 */
        if (fgets_stdin(&data->data, STDIN_MAX_SIZE) == 0) {
            printReaderError(TooMuchInputError, data->parser, err);
            *mode = READER_MODE_ERROR;
            return 0;
        }

        data->index = 0;
        data->len = STR_LEN(data->data);
    }

    if (data->index + len > data->len)  // 超出长度范围
        len = data->len - data->index;
    memcpy(dest, data->data + data->index, len);
    data->index += len;
    return len;
}

static void destructStdin(struct readerDataStdin *data) {
    free(data->data);
    if (data->sig_int != SIG_ERR)
        signal(SIGINT, data->sig_int);
    if (data->sig_term != SIG_ERR)
        signal(SIGTERM, data->sig_term);
}

static void initStdinReader(af_Parser *parser, struct readerDataStdin *data) {
    stdin_interrupt = 0;
    setStdinSignalFunc(data);
    data->parser = parser;
}

af_Parser *makeParserByStdin(FilePath file){
    if (CLEAR_FERROR(stdin))
        return NULL;

    DLC_SYMBOL(readerFunc) read_func = MAKE_SYMBOL(readFuncStdin, readerFunc);
    DLC_SYMBOL(destructReaderFunc) destruct = MAKE_SYMBOL(destructStdin, destructReaderFunc);
    af_Parser *parser = makeParser(file, read_func, destruct, sizeof(struct readerDataStdin));
    initStdinReader(parser, parser->reader->data);
    initParser(parser);
    FREE_SYMBOL(read_func);
    FREE_SYMBOL(destruct);
    return parser;
}