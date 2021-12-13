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
        char prefix=NUL;

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

        aFuntool::FileLine line;
        aFuntool::FilePath file;
    public:

        explicit Code(FileLine line, ConstFilePath file="");
        Code (const std::string &element, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        Code (BlockType block_type, Code *son, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        ~Code();

        Code *connect(Code *code);
        void destructAll();
        void display() const;
        void displayAll() const;
        bool write_v1(FILE *f, bool debug=false) const;
        bool writeAll_v1(FILE *f, bool debug=false) const;
        Code *read_v1(FILE *f, bool debug=false, int8_t read_type=code_element, bool to_son=false);
        bool readAll_v1(FILE *f, bool debug=false);
        [[nodiscard]] std::string getMD5_v1() const;
        [[nodiscard]] std::string getMD5All_v1() const;
        bool writeByteCode(ConstFilePath file_path, bool debug=false) const;  // NOLINT 允许忽略返回值
        bool readByteCode(ConstFilePath file_path);

        [[nodiscard]] CodeType getType() const {return type;}
        [[nodiscard]] char getPrefix() const {return prefix;}

        [[nodiscard]] const char *getElement() const {if (type != code_element) return ""; return element;}
        [[nodiscard]] BlockType getBlockType() const {if (type != code_block) return block_p; return block_type;}
        [[nodiscard]] Code *getSon() const {if (type != code_block) return nullptr; return son;}

        [[nodiscard]] Code *toNext() const {return next;}
        [[nodiscard]] Code *toPrev() const {return prev;}
        [[nodiscard]] Code *toFather() const {return father;}

        [[nodiscard]] aFuntool::FileLine getFileLine() const {return line;}
        [[nodiscard]] aFuntool::StringFilePath getFilePath() const {return file;}
    };
}

#endif //AFUN_CODE_H
