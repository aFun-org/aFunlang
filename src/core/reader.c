#include "__reader.h"

af_Reader *makeReader(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size) {
    af_Reader *reader = calloc(1, sizeof(af_Reader));
    reader->read_func = COPY_SYMBOL(read_func, readerFunc);
    reader->destruct = COPY_SYMBOL(destruct_func, destructReaderFunc);

    reader->data = calloc(1, data_size);
    reader->data_size = data_size;

    reader->buf = NEW_STR(DEFAULT_BUF_SIZE);
    reader->buf_size = DEFAULT_BUF_SIZE;
    reader->buf_end = reader->buf + DEFAULT_BUF_SIZE;  // 执行buf[DEFAULT_BUF_SIZE], 即NUL
    reader->read = reader->buf;
    return reader;
}

af_Reader *initReader(af_Reader *reader) {
    if (reader->init)
        return reader;
    char *new = readWord(reader->buf_size, reader);  // 写入数据
    free(new);
    reader->init = true;
    reader->line = 1;
    return reader;
}

void freeReader(af_Reader *reader) {
    if (reader->destruct != NULL)
        GET_SYMBOL(reader->destruct)(reader->data);
    free(reader->data);
    free(reader->buf);
    FREE_SYMBOL(reader->read_func);
    FREE_SYMBOL(reader->destruct);
    free(reader);
}

void *getReaderData(af_Reader *reader) {
    return reader->data;
}

char *readWord(size_t del_index, af_Reader *reader) {
    char *re;
    char *write = reader->buf_end - del_index;  // 数据写入的位置
    reader->read = reader->buf;  // 重置指针

    if (del_index == 0)
        return strCopy(NULL);  // 返回空字符串

    re = NEW_STR(del_index);
    memcpy(re, reader->buf, del_index);  // 复制旧字符串
    memmove(reader->buf, reader->buf + del_index, reader->buf_size - del_index + 1);  // +1是为了涵盖NUL
    if (!reader->read_end) { // 没到尾部, 则写入数据
        size_t len = GET_SYMBOL(reader->read_func)(reader->data, write, del_index);
        if (len > del_index)
            len = del_index;
        else if (len < del_index)
            reader->read_end = true;
        *(write + len) = NUL;
    }

    /* 计算行号 */
    for (char *tmp = re; *tmp != NUL; tmp ++) {
        if (*tmp == '\n')
            reader->line++;
    }

    return re;
}

char getChar(af_Reader *reader) {
    char ch = *(reader->read);
    if (ch != NUL) {  // 未读取到末尾
        reader->read++;
        return ch;
    } else if (reader->read_end)  // 读取到末尾, 且无新内容
        return NUL;

    char *new_buf = NEW_STR(reader->buf_size + NEW_BUF_SIZE);
    memcpy(new_buf, reader->buf, reader->buf_size);

    size_t len = GET_SYMBOL(reader->read_func)(reader->data, new_buf + reader->buf_size, NEW_BUF_SIZE);
    if (len > NEW_BUF_SIZE)
        len = NEW_BUF_SIZE;
    else if (len < NEW_BUF_SIZE)
        reader->read_end = true;
    *(new_buf + reader->buf_size + len + 1) = NUL;

    free(reader->buf);
    reader->buf = new_buf;
    reader->buf_size = reader->buf_size + NEW_BUF_SIZE;
    reader->buf_end = reader->buf + reader->buf_size;
    reader->read = reader->buf + reader->buf_size - NEW_BUF_SIZE;

    ch = *(reader->read);
    if (ch != NUL)
        reader->read++;
    return ch;
}
