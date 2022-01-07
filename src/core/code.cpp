﻿#include "code.h"
#include "init.h"
using namespace aFuncore;
using namespace aFuntool;

/**
 * 创建 `start` 代码块
 * @param line
 * @param file
 */
Code::Code(FileLine line, ConstFilePath file){  // NOLINT 不初始化 element, block_type, son
    this->type = code_start;
    if (file.empty()) {
        errorLog(aFunCoreLogger, "Make `start` code without FilePath");
        this->file = nullptr;
    } else
        this->file = strCopy(file.c_str());
    this->line = line;
}

/**
 * 创建 `element` 代码块
 * @param element
 * @param line
 * @param file
 * @param prefix
 */
aFuncore::Code::Code(const std::string &element, FileLine line, ConstFilePath file, char prefix){  // NOLINT 不初始化 block_type, son
    this->type=code_element;
    this->prefix = prefix;
    if (file.empty())
        this->file = nullptr;
    else
        this->file = strCopy(file.c_str());
    this->line = line;

    if (!isCharUTF8(element)) {
        errorLog(aFunCoreLogger, "Element not utf-8");
        this->element = nullptr;
    } else
        this->element = strCopy(element.c_str());
}

/**
 * 创建 `block` 代码块
 * @param block_type
 * @param son
 * @param line
 * @param file
 * @param prefix
 */
Code::Code(BlockType block_type, Code *son, FileLine line, ConstFilePath file, char prefix){  // NOLINT 不出时候 element
    this->type=code_block;
    this->prefix = prefix;
    if (file.empty())
        this->file = nullptr;
    else
        this->file = strCopy(file.c_str());
    this->line = line;

    this->block_type = block_type;
    this->son = son;

    for (Code *tmp = son; tmp != nullptr; tmp = tmp->next)
        tmp->father = this;
}

Code::~Code(){
    if (type == code_element)
        free(element);
    free(file);
}

/**
 * 连结代码块
 * @param code
 * @return
 */
Code *Code::connect(Code *code){
    Code *tmp = this;
    while (tmp->next != nullptr)
        tmp = tmp->next;

    if (code->type == code_start) {
        errorLog(aFunCoreLogger, "Code connect with `start`");
        return tmp;
    }

    Code *father_ = tmp->father;
    tmp->next = code;
    code->prev = tmp;
    while (code->next != nullptr) {
        code = code->next;
        code->father = father_;
    }
    return code;
}

/**
 * 删除自己以及其子、兄代码块
 */
void Code::destruct(Code *code){
    if (code->type != code_start) {
        errorLog(aFunCoreLogger, "Code delete did not with `start`");
        return;
    }

    Code *next_tmp;
    while (code != nullptr) {
        if (code->type != code_block || code->son == nullptr) {
            if (code->next == nullptr) {
                if (code->father == nullptr)
                    next_tmp = nullptr;
                else {
                    next_tmp = code->father;
                    next_tmp->son = nullptr;
                }
            } else
                next_tmp = code->next;
            delete code;
            code = next_tmp;
        } else
            code = code->son;
    }
    delete code;
}

/**
 * 显式代码块内容
 */
void Code::display() const {
    printf_stdout(0, "%c[father: %p] type=%d %p", prefix == NUL ? '-' : prefix, father, type, this);
    if (type == code_element)
        printf_stdout(0, " element: %s\n", element);
    else if (type == code_block)
        printf_stdout(0, " block: '%c' son: %p\n", block_type, son);
    else
        printf_stdout(0, "\n");
}

/**
 * 显式自己以及其子、兄代码块
 */
void Code::displayAll() const {
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code dsplay all did not with `start`");
        return;
    }

    const Code *tmp = this;
    while (tmp != nullptr) {
        tmp->display();
        if (tmp->type == code_block && tmp->son != nullptr){
            tmp = tmp->son;
            continue;
        }

        if (tmp->next == nullptr) {
            do {
                tmp = tmp->father;
            } while(tmp != nullptr && tmp->next == nullptr);
            if (tmp == nullptr)
                break;
            tmp = tmp->next;
        } else
            tmp = tmp->next;
    }
}

#define Done(write) do{if(!(write)){return false;}}while(0)

/**
 * 将code写入到文件中 (版本: 1)
 * @param f
 * @param debug 是否记录 debug 信息
 * @return
 */
bool Code::write_v1(FILE *f, bool debug) const{
    switch (type) {
        case code_element:
            Done(byteWriteInt(f, (int8_t)code_element));
            Done(byteWriteInt(f, (int8_t)prefix));
            Done(byteWriteStr(f, element));
            break;
        case code_block:
            if (son == nullptr)
                Done(byteWriteInt(f, (int8_t)4));  // 空 block 标注为 4
            else
                Done(byteWriteInt(f, (int8_t) code_block));
            Done(byteWriteInt(f, (int8_t)prefix));
            Done(byteWriteInt(f, (int8_t)block_type));
            break;
        default:
            break;

    }
    if (debug) {
        Done(byteWriteInt(f, (int16_t)line));
        Done(byteWriteStr(f, file));
    }
    return true;
}

/**
 * 将的子、兄code写入到文件中 (版本: 1)
 * 注意: 不包括自己(`start`)
 * @param f
 * @param debug
 * @return
 */
bool Code::writeAll_v1(FILE *f, bool debug) const{
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code write all did not with `start`");
        return false;
    }

    const Code *tmp = this;
    while (tmp != nullptr) {
        Done(tmp->write_v1(f, debug));
        if (tmp->type == code_block && tmp->son != nullptr){
            tmp = tmp->son;
            continue;
        }

        if (tmp->next == nullptr) {
            do {
                tmp = tmp->father;
                Done(byteWriteInt(f, (int8_t)3));
            } while(tmp != nullptr && tmp->next == nullptr);
            if (tmp == nullptr)
                break;
            tmp = tmp->next;
        } else
            tmp = tmp->next;
    }
    Done(byteWriteInt(f, (int8_t)0));
    return true;
}

/**
 * 读取文件中的code (版本: 1)
 * @param f
 * @param debug 文件是否包含 debug 信息
 * @return
 */
bool Code::readAll_v1(FILE *f, bool debug) {
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code read all did not with `start`");
        return false;
    }

    Code *father_ = nullptr;
    Code *next_ = this;
    const Code *tmp = this;
    while (tmp != nullptr) {
        int8_t type_ = NUL;
        Done(byteReadInt(f, &type_));
        switch (type_) {
            case 0:
                goto RETURN;
            case 3:
                if (next_ == nullptr) {
                    errorLog(aFunCoreLogger, "Code read all error");
                    return false;
                }
                next_ = next_->father;
                break;
            default: {
                Code *ret;
                if (next_ == nullptr && father_ != nullptr)
                    ret = father_->read_v1(f, debug, type_, true);
                else if (next_ != nullptr)
                    ret = next_->read_v1(f, debug, type_, false);
                else {
                    errorLog(aFunCoreLogger, "Code read all error");
                    return false;
                }

                if (ret == nullptr) {
                    errorLog(aFunCoreLogger, "Code read error");
                    return false;
                } else if (type_ == code_block) {
                    next_ = nullptr;
                    father_ = ret;
                } else {
                    next_ = ret;
                    father_ = nullptr;
                }
                break;
            }
        }
    }
RETURN:
    return true;
}

#undef Done
#define Done(write) do{if(!(write)){return nullptr;}}while(0)

/**
 * 读取 code 并拼接到 next 或 son 中 (版本: 1)
 * @param f
 * @param debug 文件是否包含 debug 信息
 * @param read_type 读取类型
 * @param to_son 若位true则拼接到son, 否则拼接到next
 * @return
 */
Code *Code::read_v1(FILE *f, bool debug, int8_t read_type, bool to_son) {
    Code *ret;
    switch (read_type) {
        case code_element: {
            int8_t prefix_ = NUL;
            std::string element_;
            Done(byteReadInt(f, &prefix_));
            Done(byteReadStr(f, element_));
            ret = Code::create(element_, 0, "", char(prefix_));
            break;
        }
        case 4:
        case code_block: {
            int8_t prefix_ = NUL;
            int8_t block_type = NUL;
            Done(byteReadInt(f, &prefix_));
            Done(byteReadInt(f, &block_type));
            ret = Code::create(BlockType(block_type), nullptr, 0, "", char(prefix_));
            break;
        }
        default:
            errorLog(aFunCoreLogger, "Read code with error type.");
            return nullptr;
    }

    if (debug) {
        int16_t line_ = NUL;
        char *file_ = nullptr;
        Done(byteReadInt(f, &line_));
        Done(byteReadStr(f, file_));
        ret->line = line;
        if (strlen(file) != 0)
            ret->file = strCopy(file);
    }

    if (to_son) {
        if (type != code_block || son != nullptr) {
            errorLog(aFunCoreLogger, "Read son with error type.");
            delete ret;
            return nullptr;
        }
        ret->father = this;
        son = ret;
    } else
        connect(ret);
    return ret;
}

#undef Done

/**
 * 计算代码的MD5值（版本：1）
 * @return md5
 */
std::string Code::getMD5_v1() const {
    char md5str[MD5_STR_LEN + 1] {};
    char md5_value[MD5_SIZE];
    MD5_CTX *md5 = MD5Init();

    char head[] = {(char)type, prefix, 'x', 'x', NUL};
    if (prefix == NUL)
        head[1] = '-';
    if (type == code_block) {
        head[2] = son == nullptr ? 'n' : 's';
        head[3] = block_type;
    }

    MD5Update(md5, (unsigned char *)head, strlen((char *)head));
    if (type == code_element)
        MD5Update(md5, (unsigned char *)element, strlen((char *)element));
    else if (type == code_block)
        MD5Update(md5, (unsigned char *)"block", 5);
    else
        MD5Update(md5, (unsigned char *)"start", 5);

    MD5Final(md5, (unsigned char *)md5_value);
    for(int i = 0; i < MD5_SIZE; i++)
        snprintf((char *)md5str + i * 2, 2 + 1, "%02x", md5_value[i]);
    return md5str;
}

/**
 * 计算代码（子、兄）的MD5值（版本：1）
 * @return md5
 */
std::string Code::getMD5All_v1() const {
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code get md5 all did not with `start`");
        return "";
    }

    char md5str[MD5_STR_LEN + 1] {};
    char md5_value[MD5_SIZE];
    MD5_CTX *md5 = MD5Init();

    const Code *tmp = this;
    while (tmp != nullptr) {
        std::string code_md5 = tmp->getMD5_v1();
        MD5Update(md5, (unsigned char *)code_md5.c_str(), code_md5.size());

        if (tmp->type == code_block && tmp->son != nullptr){
            tmp = tmp->son;
            continue;
        }

        if (tmp->next == nullptr) {
            do {
                tmp = tmp->father;
            } while(tmp != nullptr && tmp->next == nullptr);
            if (tmp == nullptr)
                break;
            tmp = tmp->next;
        } else
            tmp = tmp->next;
    }

    MD5Final(md5, (unsigned char *)md5_value);
    for(int i = 0; i < MD5_SIZE; i++)
        snprintf((char *)md5str + i * 2, 2 + 1, "%02x", md5_value[i]);
    return md5str;
}

static const std::string ByteCodeHead = "aFunByteCode";  // NOLINT
static const int MaxByteCodeVersion = 1;  // 字节码版本号, 有别于 aFun 版本号

#define Done(write) do{if(!(write)){goto RETURN_FALSE;}}while(0)

/**
 * 生成字节码文件（版本: MaxByteCodeVersion）
 * @param file_path
 * @param debug
 * @return
 */
bool Code::writeByteCode(ConstFilePath file_path, bool debug) const {
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "ByteCode write all did not with `start`");
        return false;
    }

    FILE *f = fileOpen(file_path, "wb");
    if (f == nullptr) {
        warningLog(aFunCoreLogger, "Write ByteCode create file error.");
        return false;
    }

    Done(byteWriteStr(f, ByteCodeHead));
    Done(byteWriteInt(f, int16_t(MaxByteCodeVersion)));
    Done(byteWriteStr(f, getMD5All_v1()));
    Done(byteWriteInt(f, int8_t(debug)));
    Done(writeAll_v1(f, debug));
    fileClose(f);
    return true;

RETURN_FALSE:
    fileClose(f);
    return false;
}

/**
 * 读取字节码文件（版本: 自动识别）
 * @param file_path
 * @return
 */
bool Code::readByteCode(ConstFilePath file_path){
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "ByteCode read all did not with `start`");
        return false;
    }

    FILE *f = fileOpen(file_path, "rb");
    if (f == nullptr) {
        warningLog(aFunCoreLogger, "Read ByteCode read file error.");
        return false;
    }

    std::string head;
    Done(byteReadStr(f, head));
    if (head != ByteCodeHead)
        return false;

    int16_t version;
    Done(byteReadInt(f, &version));
    switch (version) {  // NOLINT 为拓展方便, 使用switch-case而不是if-else
        case 1: {
            std::string md5;
            int8_t debug;
            Done(byteReadStr(f, md5));
            Done(byteReadInt(f, &debug));

            Done(readAll_v1(f, debug));
            std::string md5_ = getMD5All_v1();
            if (md5_ != md5)
                goto RETURN_FALSE;
            return true;
        }
        default:
            goto RETURN_FALSE;
    }
    fileClose(f);
    return true;

RETURN_FALSE:
    fileClose(f);
    return false;
}

#undef Done