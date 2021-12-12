#include "code.hpp"
#include "init.hpp"
using namespace aFuncore;
using namespace aFuntool;

Code::Code(FileLine line, ConstFilePath file){
    this->type = code_start;
    this->file = file;
    this->line = line;
}

aFuncore::Code::Code(const std::string &element, FileLine line, ConstFilePath file, char prefix){
    this->type=code_element;
    this->perfix = prefix;
    this->file = file;
    this->line = line;

    if (!isCharUTF8(element)) {
        errorLog(aFunCoreLogger, "Element not utf-8");
        this->element = nullptr;
    } else
        this->element = strCopy(element.c_str());
}

Code::Code(BlockType block_type, Code *son, FileLine line, ConstFilePath file, char prefix){
    this->type=code_block;
    this->perfix = prefix;
    this->file = file;
    this->line = line;

    this->block_type = block_type;
    this->son = son;

    for (Code *tmp = son; tmp != nullptr; tmp = tmp->next)
        tmp->father = this;
}

Code::~Code(){
    if (type == code_element)
        safeFree(element);
}

Code *Code::connect(Code *code){
    Code *tmp = this;
    while (tmp->next != nullptr)
        tmp = tmp->next;

    if (code->type == code_start) {
        errorLog(aFunCoreLogger, "Code connect with `start`");
        return tmp;
    }

    tmp->next = code;
    code->prev = tmp;
    while (code->next != nullptr)
        code = code->next;
    return code;
}

void Code::destruct(){
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code delete did not with `start`");
        return;
    }

    Code *tmp = this;
    while (tmp->next != nullptr || tmp->father != nullptr) {
        if (tmp->type == code_element || tmp->son == nullptr)
            delete tmp;
        else {
            tmp = tmp->son;
            tmp->father->son = nullptr;
            continue;
        }

        if (tmp->next == nullptr)
            tmp = tmp->father;
        else
            tmp = tmp->next;

    }
    delete tmp;
}

void Code::display(){
    printf_stdout(0, "%c[father: %p] type=%d %p", perfix == NUL ? '=' : perfix, father, type, this);
    if (type == code_element)
        printf_stdout(0, "element: %s\n", element);
    else
        printf_stdout(0, "block: %c son: %p\n", block_type, son);
}

void Code::displayAll(){
    if (this->type != code_start) {
        errorLog(aFunCoreLogger, "Code dsplay all did not with `start`");
        return;
    }

    Code *tmp = this;
    while (tmp->next != nullptr || tmp->father != nullptr) {
        tmp->display();
        if (tmp->type == code_block && tmp->son != nullptr){
            tmp = tmp->son;
            continue;
        }

        if (tmp->next == nullptr)
            tmp = tmp->father->next;
        else
            tmp = tmp->next;
    }
    tmp->display();
}