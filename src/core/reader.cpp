#include "core_init.hpp"
#include "__reader.hpp"
static void readFirstWord(af_Reader *reader);

af_Reader *makeReader(FileLine line, ConstFilePath file, DLC_SYMBOL(readerFunc) read_func,
                      DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size){
    auto reader = calloc(1, af_Reader);
    reader->read_func = COPY_SYMBOL(read_func, readerFunc);
    reader->destruct = COPY_SYMBOL(destruct_func, destructReaderFunc);

    reader->data = calloc_size(1, data_size);
    reader->data_size = data_size;

    reader->buf = NEW_STR(DEFAULT_BUF_SIZE);
    reader->buf_size = DEFAULT_BUF_SIZE;  // buf_size 不包括NUL
    reader->read = reader->buf;

    reader->line = line;
    reader->file = strCopy(file);
    return reader;
}

af_Reader *initReader(af_Reader *reader) {
    if (reader->init)
        return reader;
    readFirstWord(reader);
    reader->init = true;
    return reader;
}

void freeReader(af_Reader *reader) {
    if (reader->destruct != nullptr)
        GET_SYMBOL(reader->destruct)(reader->data);
    free(reader->data);
    free(reader->buf);
    free(reader->file);
    FREE_SYMBOL(reader->read_func);
    FREE_SYMBOL(reader->destruct);
    free(reader);
}

void *getReaderData(af_Reader *reader) {
    return reader->data;
}

char *readWord(size_t del_index, af_Reader *reader) {
    char *re;
    int mode = READER_MODE_NORMAL;
    reader->read = reader->buf;  // 重置指针

    if (del_index == 0)
        return strCopy(nullptr);  // 返回空字符串

    re = NEW_STR(del_index);
    memcpy(re, reader->buf, del_index);  // 复制旧字符串
    memmove(reader->buf, reader->buf + del_index, reader->buf_size - del_index + 1);  // +1是为了涵盖NUL

    if (!reader->read_end) { // 没到尾部, 则写入数据
        char *write = reader->buf + STR_LEN(reader->buf);  // 数据写入的位置
        size_t len_ = reader->buf_size - STR_LEN(reader->buf);
        size_t len = GET_SYMBOL(reader->read_func)(reader->data, write, len_, &mode);
        if (len > len_)
            len = len_;
        *(write + len) = NUL;
    }

    if (mode == READER_MODE_FINISHED)
        reader->read_end = true;
    else if (mode == READER_MODE_ERROR) {
        reader->read_end = true;
        reader->read_error = true;
    }

    /* 计算行号 */
    for (char *tmp = re; *tmp != NUL; tmp ++) {
        if (*tmp == '\n')
            reader->line++;
    }

    if (!isCharUTF8(re)) {
        free(re);
        writeErrorLog(aFunCoreLogger, "Is not utf-8");
        return nullptr;
    }

    return re;
}

static void readFirstWord(af_Reader *reader) {
    int mode = READER_MODE_NORMAL;
    reader->read = reader->buf;  // 重置指针

    char *write = reader->buf + STR_LEN(reader->buf);  // 数据写入的位置
    size_t len_ = reader->buf_size - STR_LEN(reader->buf);
    size_t len = GET_SYMBOL(reader->read_func)(reader->data, write, len_, &mode);
    if (len > len_)
        len = len_;
    *(write + len) = NUL;

    if (mode == READER_MODE_FINISHED)
        reader->read_end = true;
    else if (mode == READER_MODE_ERROR) {
        reader->read_end = true;
        reader->read_error = true;
    }
}

char getChar(af_Reader *reader) {
    char ch = *(reader->read);
    if (ch != NUL) {  // 未读取到末尾
        reader->read++;
        return ch;
    } else if (reader->read_end)  // 读取到末尾, 且无新内容
        return NUL;

    if (reader->read == reader->buf + reader->buf_size) {
        char *new_buf = NEW_STR(reader->buf_size + NEW_BUF_SIZE);
        memcpy(new_buf, reader->buf, reader->buf_size);

        int mode = READER_MODE_NORMAL;
        size_t len = GET_SYMBOL(reader->read_func)(reader->data, new_buf + reader->buf_size, NEW_BUF_SIZE, &mode);
        if (len > NEW_BUF_SIZE)
            len = NEW_BUF_SIZE;
        *(new_buf + reader->buf_size + len) = NUL;

        if (mode == READER_MODE_FINISHED)
            reader->read_end = true;
        else if (mode == READER_MODE_ERROR) {
            reader->read_end = true;
            reader->read_error = true;
        }

        free(reader->buf);
        reader->buf = new_buf;
        reader->buf_size = reader->buf_size + NEW_BUF_SIZE;
        reader->read = reader->buf + reader->buf_size - NEW_BUF_SIZE;  // 当前读取的位置
    } else {
        int mode = READER_MODE_NORMAL;
        size_t len_ = reader->buf_size - (reader->read - reader->buf);  // 总长度  - (已读取长度) = 剩余空白
        size_t len = GET_SYMBOL(reader->read_func)(reader->data, reader->read, len_, &mode);
        if (len > len_)
            len = len_;
        *(reader->read + len) = NUL;
    }

    ch = *(reader->read);
    if (ch != NUL)
        reader->read++;
    return ch;
}
