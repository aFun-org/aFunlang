#ifndef AFUN_CODE_H
#define AFUN_CODE_H
#include "iostream"
#include "tool.hpp"
#include "exception.hpp"

namespace aFuncore {
    typedef enum CodeType {
        code_start = 0,
        code_element = 1,
        code_block = 2,
    } CodeType;

    typedef enum BlockType {
        block_p = '(',
        block_b = '[',
        block_c = '{',
    } BlockType;

    typedef class Code Code;
    class Code {
        CodeType type;
        char perfix=NUL;

        union {
            char *element;  // union 内不使用 std::string

            struct {
                BlockType block_type;
                Code *son;
            };
        };

        Code *father = nullptr;;
        Code *next = nullptr;;
        Code *prev = nullptr;;

    public:
        aFuntool::FileLine line;
        aFuntool::FilePath file;

        explicit Code(FileLine line, ConstFilePath file="");
        Code (const std::string &element, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        Code (BlockType block_type, Code *son, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        ~Code();

        Code *connect(Code *code);
        void destruct();
        void display();
        void displayAll();

        [[nodiscard]] CodeType getType() const {return type;}
        [[nodiscard]] char getPrefix() const {return perfix;}

        [[nodiscard]] const char *getElement() const {if (type != code_element) throw aFuncore::AttributesError("Code.Element"); return element;}
        [[nodiscard]] BlockType getBlockType() const {if (type != code_block) throw aFuncore::AttributesError("Code.BlockType"); return block_type;}
        [[nodiscard]] Code *getSon() const {if (type != code_block) return nullptr; return son;}

        [[nodiscard]] Code *toNext() const {return next;}
        [[nodiscard]] Code *toPrev() const {return prev;}
        [[nodiscard]] Code *toFather() const {return father;}

    };
}

#endif //AFUN_CODE_H
