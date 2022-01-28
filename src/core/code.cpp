#include "code.h"
#include "init.h"

namespace aFuncore {
    Code::~Code(){
        ByteCode *next_tmp;
        while (code != nullptr) {
            if (code->type != ByteCode::code_block || code->data.son == nullptr) {
                if (code->next == nullptr) {
                    if (code->father == nullptr)
                        next_tmp = nullptr;
                    else {
                        next_tmp = code->father;
                        next_tmp->data.son = nullptr;
                    }
                } else
                    next_tmp = code->next;
                delete code;
                code = next_tmp;
            } else
                code = code->data.son;
        }
        delete code;
    }

    /**
     * 显式自己以及其子、兄代码块
     */
    void Code::display() const{
        if (code->type != ByteCode::code_start) {
            errorLog(aFunCoreLogger, "Code dsplay all did not with `start`");
            return;
        }

        const Code::ByteCode *tmp = code;
        while (tmp != nullptr) {
            tmp->display();
            if (tmp->type == ByteCode::code_block && tmp->data.son != nullptr) {
                tmp = tmp->data.son;
                continue;
            }

            if (tmp->next == nullptr) {
                do {
                    tmp = tmp->father;
                } while (tmp != nullptr && tmp->next == nullptr);
                if (tmp == nullptr)
                    break;
                tmp = tmp->next;
            } else
                tmp = tmp->next;
        }
    }

#define Done(write) do{if(!(write)){return false;}}while(0)
    /**
     * 将的子、兄code写入到文件中 (版本: 1)
     * 注意: 不包括自己(`start`)
     * @param f
     * @param debug
     * @return
     */
    bool Code::write_v1(FILE *f, bool debug) const{
        if (code->type != ByteCode::code_start) {
            errorLog(aFunCoreLogger, "Code write all did not with `start`");
            return false;
        }

        const Code::ByteCode *tmp = code;
        while (tmp != nullptr) {
            Done(tmp->write_v1(f, debug));
            if (tmp->type == ByteCode::code_block && tmp->data.son != nullptr) {
                tmp = tmp->data.son;
                continue;
            }

            if (tmp->next == nullptr) {
                do {
                    tmp = tmp->father;
                    Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(3)));
                } while (tmp != nullptr && tmp->next == nullptr);
                if (tmp == nullptr)
                    break;
                tmp = tmp->next;
            } else
                tmp = tmp->next;
        }
        Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(0)));
        return true;
    }

    /**
     * 读取文件中的code (版本: 1)
     * @param f
     * @param debug 文件是否包含 debug 信息
     * @return
     */
    bool Code::read_v1(FILE *f, bool debug) {
        if (code->type != ByteCode::code_start) {
            errorLog(aFunCoreLogger, "Code read all did not with `start`");
            return false;
        }

        Code::ByteCode *father_ = nullptr;
        Code::ByteCode *next_ = code;
        const Code::ByteCode *tmp = code;
        while (tmp != nullptr) {
            int8_t type_ = aFuntool::NUL;
            Done(aFuntool::byteReadInt(f, &type_));
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
                    Code::ByteCode *ret = nullptr;
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
                    } else if (type_ == ByteCode::code_block) {
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

    /**
     * 计算代码（子、兄）的MD5值（版本：1）
     * @return md5
     */
    std::string Code::getMD5_v1() const{
        if (code->type != ByteCode::code_start) {
            errorLog(aFunCoreLogger, "Code get md5 all did not with `start`");
            return "";
        }

        char md5str[aFuntool::MD5_STR_LEN + 1]{};
        char md5_value[aFuntool::MD5_SIZE];
        aFuntool::MD5_CTX *md5 = aFuntool::MD5Init();

        const Code::ByteCode *tmp = code;
        while (tmp != nullptr) {
            std::string code_md5 = tmp->getMD5_v1();
            MD5Update(md5, (unsigned char *) code_md5.c_str(), code_md5.size());

            if (tmp->type == ByteCode::code_block && tmp->data.son != nullptr) {
                tmp = tmp->data.son;
                continue;
            }

            if (tmp->next == nullptr) {
                do {
                    tmp = tmp->father;
                } while (tmp != nullptr && tmp->next == nullptr);
                if (tmp == nullptr)
                    break;
                tmp = tmp->next;
            } else
                tmp = tmp->next;
        }

        MD5Final(md5, (unsigned char *) md5_value);
        for (int i = 0; i < aFuntool::MD5_SIZE; i++)
            snprintf((char *) md5str + i * 2, 2 + 1, "%02x", md5_value[i]);
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
    bool Code::writeByteCode(const aFuntool::FilePath &file_path, bool debug) const{
        if (code->type != ByteCode::code_start) {
            errorLog(aFunCoreLogger, "ByteCode write all did not with `start`");
            return false;
        }

        FILE *f = aFuntool::fileOpen(file_path, "wb");
        if (f == nullptr) {
            warningLog(aFunCoreLogger, "Write ByteCode create file error.");
            return false;
        }

        Done(aFuntool::byteWriteStr(f, ByteCodeHead));
        Done(aFuntool::byteWriteInt(f, int16_t(MaxByteCodeVersion)));
        Done(aFuntool::byteWriteStr(f, getMD5_v1()));
        Done(aFuntool::byteWriteInt(f, int8_t(debug)));
        Done(write_v1(f, debug));
        aFuntool::fileClose(f);
        return true;

RETURN_FALSE:
        aFuntool::fileClose(f);
        return false;
    }

    /**
     * 读取字节码文件（版本: 自动识别）
     * @param file_path
     * @return
     */
    bool Code::readByteCode(const aFuntool::FilePath &file_path){
        if (code->type != ByteCode::code_start) {
            errorLog(aFunCoreLogger, "ByteCode read all did not with `start`");
            return false;
        }

        FILE *f = aFuntool::fileOpen(file_path, "rb");
        if (f == nullptr) {
            warningLog(aFunCoreLogger, "Read ByteCode read file error.");
            return false;
        }

        std::string head;
        Done(aFuntool::byteReadStr(f, head));
        if (head != ByteCodeHead)
            return false;

        int16_t version;
        Done(aFuntool::byteReadInt(f, &version));
        switch (version) {  // NOLINT 为拓展方便, 使用switch-case而不是if-else
            case 1: {
                std::string md5;
                int8_t debug;
                Done(aFuntool::byteReadStr(f, md5));
                Done(aFuntool::byteReadInt(f, &debug));

                Done(read_v1(f, debug));
                std::string md5_ = getMD5_v1();
                if (md5_ != md5)
                    goto RETURN_FALSE;
                return true;
            }
            default:
                goto RETURN_FALSE;
        }
        aFuntool::fileClose(f);
        return true;

RETURN_FALSE:
        aFuntool::fileClose(f);
        return false;
    }

#undef Done

    /**
     * 创建 `start` 代码块
     * @param line
     * @param file
     */
    Code::ByteCode::ByteCode(Code &belong_, aFuntool::FileLine line) : belong{belong_} {  // NOLINT 不初始化 element, block_type, son
        this->type = code_start;
        this->line = line;
    }

    /**
     * 创建 `element` 代码块
     * @param element
     * @param line
     * @param file
     * @param prefix
     */
    Code::ByteCode::ByteCode(Code &belong_, const std::string &element, aFuntool::FileLine line,
                             char prefix) : belong{belong_}{  // NOLINT 不初始化 block_type, son
        this->type = code_element;
        this->prefix = prefix;
        this->line = line;

        if (!aFuntool::isCharUTF8(element)) {
            errorLog(aFunCoreLogger, "Element not utf-8");
            this->data.element = nullptr;
        } else
            this->data.element = aFuntool::strCopy(element.c_str());
    }

    /**
     * 创建 `block` 代码块
     * @param block_type
     * @param son
     * @param line
     * @param file
     * @param prefix
     */
    Code::ByteCode::ByteCode(Code &belong_, BlockType block_type, Code::ByteCode *son, aFuntool::FileLine line,
                             char prefix) : belong{belong_} {  // NOLINT 不出时候 element
        this->type = code_block;
        this->prefix = prefix;
        this->line = line;

        this->data.block_type = block_type;
        this->data.son = son;

        for (Code::ByteCode *tmp = son; tmp != nullptr; tmp = tmp->next)
            tmp->father = this;
    }

    Code::ByteCode::~ByteCode(){
        if (type == code_element)
            aFuntool::safeFree(data.element);
    }

    /**
     * 连结代码块
     * @param new_code
     * @return
     */
    Code::ByteCode *Code::ByteCode::connect(Code::ByteCode *new_code){
        Code::ByteCode *tmp = this;
        while (tmp->next != nullptr)
            tmp = tmp->next;

        if (new_code == nullptr)
            return tmp;

        if (new_code->type == code_start) {
            errorLog(aFunCoreLogger, "Code connect with `start`");
            return tmp;
        }

        if (&new_code->belong != &tmp->belong) {
            errorLog(aFunCoreLogger, "Code connect with difference belong");
            return tmp;
        }

        Code::ByteCode *father_ = tmp->father;
        tmp->next = new_code;
        new_code->prev = tmp;
        while (new_code->next != nullptr) {
            new_code = new_code->next;
            new_code->father = father_;
        }
        return new_code;
    }

    /**
     * 显式代码块内容
     */
    void Code::ByteCode::display() const{
        aFuntool::cout << (prefix == aFuntool::NUL ? '-' : prefix) << "[father: " << father << "] type=" << type << " " << this;
        if (type == code_element)
            aFuntool::cout << " element: " << data.element << "\n";
        else if (type == code_block)
            aFuntool::cout << " block: '" << data.block_type << "' son: " << data.son << "\n";
        else
            aFuntool::cout << "\n";
    }

#define Done(write) do{if(!(write)){return false;}}while(0)

    /**
     * 将code写入到文件中 (版本: 1)
     * @param f
     * @param debug 是否记录 debug 信息
     * @return
     */
    bool Code::ByteCode::write_v1(FILE *f, bool debug) const{
        switch (type) {
            case code_element:
                Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(code_element)));
                Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(prefix)));
                Done(aFuntool::byteWriteStr(f, (data.element)));
                break;
            case code_block:
                if (data.son == nullptr)
                    Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(4)));  // 空 block 标注为 4
                else
                    Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(code_block)));
                Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(prefix)));
                Done(aFuntool::byteWriteInt(f, static_cast<int8_t>(data.block_type)));
                break;
            default:
                break;

        }
        if (debug)
            Done(aFuntool::byteWriteInt(f, static_cast<int16_t>(line)));
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
    Code::ByteCode *Code::ByteCode::read_v1(FILE *f, bool debug, int8_t read_type, bool to_son){
        Code::ByteCode *ret;
        switch (read_type) {
            case code_element: {
                int8_t prefix_ = aFuntool::NUL;
                std::string element_;
                Done(aFuntool::byteReadInt(f, &prefix_));
                Done(aFuntool::byteReadStr(f, element_));
                ret = new Code::ByteCode(belong, element_, 0, char(prefix_));
                break;
            }
            case 4:
            case code_block: {
                int8_t prefix_ = aFuntool::NUL;
                int8_t block_type = aFuntool::NUL;
                Done(aFuntool::byteReadInt(f, &prefix_));
                Done(aFuntool::byteReadInt(f, &block_type));
                ret = new Code::ByteCode(belong, BlockType(block_type), nullptr, 0, char(prefix_));
                break;
            }
            default:
                errorLog(aFunCoreLogger, "Read code with error type.");
                return nullptr;
        }

        if (debug) {
            int16_t line_ = aFuntool::NUL;
            Done(aFuntool::byteReadInt(f, &line_));
            ret->line = line;
        }

        if (to_son) {
            if (type != code_block || data.son != nullptr) {
                errorLog(aFunCoreLogger, "Read son with error type.");
                delete ret;
                return nullptr;
            }
            ret->father = this;
            data.son = ret;
        } else
            connect(ret);
        return ret;
    }

#undef Done

    /**
     * 计算代码的MD5值（版本：1）
     * @return md5
     */
    std::string Code::ByteCode::getMD5_v1() const{
        char md5str[aFuntool::MD5_STR_LEN + 1]{};
        char md5_value[aFuntool::MD5_SIZE];
        aFuntool::MD5_CTX *md5 = aFuntool::MD5Init();

        char head[] = {static_cast<char>(type), prefix, 'x', 'x', aFuntool::NUL};
        if (prefix == aFuntool::NUL)
            head[1] = '-';
        if (type == code_block) {
            head[2] = data.son == nullptr ? 'n' : 's';
            head[3] = data.block_type;
        }

        MD5Update(md5, (unsigned char *) head, strlen((char *) head));
        if (type == code_element)
            MD5Update(md5, (unsigned char *) data.element, strlen((char *) data.element));
        else if (type == code_block)
            MD5Update(md5, (unsigned char *) "block", 5);
        else
            MD5Update(md5, (unsigned char *) "start", 5);

        MD5Final(md5, (unsigned char *) md5_value);
        for (int i = 0; i < aFuntool::MD5_SIZE; i++)
            snprintf((char *) md5str + i * 2, 2 + 1, "%02x", md5_value[i]);
        return md5str;
    }
}