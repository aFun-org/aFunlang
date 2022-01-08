﻿#ifndef AFUN_CODE_H
#define AFUN_CODE_H
#include "aFuntool.h"
#include "aFunCoreExport.h"
#include "core.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Code {
        CodeType type;
        char prefix=NUL;

        union {
            char *element = nullptr;  // union 内不使用 std::string
            struct {  // NOLINT 不需要初始化
                BlockType block_type;
                Code *son;
            };
        };

        Code *father = nullptr;;
        Code *next = nullptr;;
        Code *prev = nullptr;;

        aFuntool::FileLine line;
        aFuntool::FilePath file;

    protected:
        explicit Code(FileLine line, ConstFilePath file="");
        Code (const std::string &element, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        Code (BlockType block_type, Code *son, aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        ~Code();
    public:
        Code(const Code &)=delete;
        Code &operator=(const Code &)=delete;

        static Code *create(FileLine line, ConstFilePath file="");
        static Code *create(const std::string &element,
                            aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        static Code *create(BlockType block_type, Code *son,
                            aFuntool::FileLine line, aFuntool::ConstFilePath file="", char prefix=NUL);
        static void destruct(Code *code);

        Code *connect(Code *code);
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

        [[nodiscard]] CodeType getType() const;
        [[nodiscard]] char getPrefix() const;

        [[nodiscard]] const char *getElement() const;
        [[nodiscard]] BlockType getBlockType() const;
        [[nodiscard]] Code *getSon() const;

        [[nodiscard]] Code *toNext() const;
        [[nodiscard]] Code *toPrev() const;
        [[nodiscard]] Code *toFather() const;

        [[nodiscard]] aFuntool::FileLine getFileLine() const;
        [[nodiscard]] aFuntool::FilePath getFilePath() const;
    };
}

#include "code.inline.h"

#endif //AFUN_CODE_H
