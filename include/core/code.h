#ifndef AFUN_CODE_H
#define AFUN_CODE_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Code {
    public:
        class ByteCode;

        inline explicit Code(aFuntool::StringFilePath file_);
        ~Code();
        Code &operator=(const Code &)=delete;

        void display() const;
        [[nodiscard]] std::string getMD5_v1() const;
        bool writeByteCode(aFuntool::ConstFilePath file_path, bool debug=false) const;  // NOLINT 允许忽略返回值
        bool readByteCode(aFuntool::ConstFilePath file_path);

        [[nodiscard]] inline aFuntool::ConstFilePath getFilePath() const;
        [[nodiscard]] inline ByteCode *getByteCode() const;
    private:
        ByteCode *code;
        aFuntool::StringFilePath file;

        bool write_v1(FILE *f, bool debug=false) const;
        bool read_v1(FILE *f, bool debug=false);
    };
    
    class AFUN_CORE_EXPORT Code::ByteCode {
        friend class Code;
    public:
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

        explicit ByteCode(Code &belong, aFuntool::FileLine line);
        ByteCode(Code &belong, const std::string &element, aFuntool::FileLine line, char prefix=aFuntool::NUL);
        ByteCode(Code &belong, BlockType block_type, ByteCode *son, aFuntool::FileLine line, char prefix=aFuntool::NUL);
        ~ByteCode();
        ByteCode &operator=(const ByteCode &)=delete;

        ByteCode *connect(ByteCode *new_code);
        void display() const;
        bool write_v1(FILE *f, bool debug=false) const;
        ByteCode *read_v1(FILE *f, bool debug=false, int8_t read_type=code_element, bool to_son=false);
        [[nodiscard]] std::string getMD5_v1() const;

        [[nodiscard]] CodeType getType() const;
        [[nodiscard]] char getPrefix() const;

        [[nodiscard]] const char *getElement() const;
        [[nodiscard]] BlockType getBlockType() const;
        [[nodiscard]] ByteCode *getSon() const;
        [[nodiscard]] aFuntool::FileLine getFileLine() const;
        [[nodiscard]] aFuntool::ConstFilePath getFilePath() const;

        [[nodiscard]] ByteCode *toNext() const;
        [[nodiscard]] ByteCode *toPrev() const;
        [[nodiscard]] ByteCode *toFather() const;

    private:
        CodeType type;
        char prefix=aFuntool::NUL;

        union CodeData {
            char *element;  // union 内不使用 std::string
            struct {  // NOLINT 不需要初始化
                BlockType block_type;
                ByteCode *son;
            };
            inline CodeData();
        } data;

        ByteCode *father = nullptr;
        ByteCode *next = nullptr;
        ByteCode *prev = nullptr;

        Code &belong;
        aFuntool::FileLine line;
    };
}

#include "code.inline.h"

#endif //AFUN_CODE_H
