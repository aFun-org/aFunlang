#ifndef AFUN_CODE_HPP
#define AFUN_CODE_HPP
#include "tool.hpp"
#include "aFunCoreExport.h"

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
        AFUN_CORE_EXPORT explicit Code(FileLine line, ConstFilePath file="");
        AFUN_CORE_EXPORT Code (const std::string &element, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        AFUN_CORE_EXPORT Code (BlockType block_type, Code *son, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        AFUN_CORE_EXPORT ~Code();

        AFUN_CORE_EXPORT Code *connect(Code *code);
        AFUN_CORE_EXPORT void destructAll();
        AFUN_CORE_EXPORT void display() const;
        AFUN_CORE_EXPORT void displayAll() const;
        AFUN_CORE_EXPORT bool write_v1(FILE *f, bool debug=false) const;
        AFUN_CORE_EXPORT bool writeAll_v1(FILE *f, bool debug=false) const;
        AFUN_CORE_EXPORT Code *read_v1(FILE *f, bool debug=false, int8_t read_type=code_element, bool to_son=false);
        AFUN_CORE_EXPORT bool readAll_v1(FILE *f, bool debug=false);
        [[nodiscard]] AFUN_CORE_EXPORT std::string getMD5_v1() const;
        [[nodiscard]] AFUN_CORE_EXPORT std::string getMD5All_v1() const;
        AFUN_CORE_EXPORT bool writeByteCode(ConstFilePath file_path, bool debug=false) const;  // NOLINT 允许忽略返回值
        AFUN_CORE_EXPORT bool readByteCode(ConstFilePath file_path);

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

#endif //AFUN_CODE_HPP
