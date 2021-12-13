#include "code.hpp"
#include "init.hpp"
using namespace aFuncore;
using namespace aFuntool;

/**
 * 创建 `start` 代码块
 * @param line
 * @param file
 */
Code::Code(FileLine line, ConstFilePath file){  // NOLINT 不初始化 element, block_type, son
    this->type = code_start;
    if (file.empty())
        this->file = nullptr;
    else
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
void Code::destructAll(){
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code delete did not with `start`");
        return;
    }

    Code *tmp = this;
    Code *next_tmp;
    while (tmp != nullptr) {
        if (tmp->type != code_block || tmp->son == nullptr) {
            if (tmp->next == nullptr) {
                if (tmp->father == nullptr)
                    next_tmp = nullptr;
                else {
                    next_tmp = tmp->father;
                    next_tmp->son = nullptr;
                }
            } else
                next_tmp = tmp->next;
            delete tmp;
            tmp = next_tmp;
        } else
            tmp = tmp->son;
    }
    delete tmp;
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
            ret = new Code(element_, 0, "", char(prefix_));
            break;
        }
        case 4:
        case code_block: {
            int8_t prefix_ = NUL;
            int8_t block_type = NUL;
            Done(byteReadInt(f, &prefix_));
            Done(byteReadInt(f, &block_type));
            ret = new Code(BlockType(block_type), nullptr, 0, "", char(prefix_));
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