#ifndef AFUN_CODE_H
#define AFUN_CODE_H
#include "aFuntool.h"
#include "aFunCoreExport.h"

namespace aFuncore {
    class AFUN_CORE_EXPORT Code {
    public:
        class ByteCode;

        AFUN_INLINE explicit Code(aFuntool::FilePath file_);
        ~Code();
        Code &operator=(const Code &)=delete;

        [[nodiscard]] std::string getMD5_v1() const;
        bool writeByteCode(const aFuntool::FilePath &file_path, bool debug=false) const;  // NOLINT 允许忽略返回值
        bool readByteCode(const aFuntool::FilePath &file_path);

#ifdef AFUN_DEBUG
        void display() const;
#endif

        [[nodiscard]] AFUN_INLINE const aFuntool::FilePath &getFilePath() const;
        [[nodiscard]] AFUN_INLINE ByteCode *getByteCode() const;
    private:
        ByteCode *code;
        aFuntool::FilePath file;

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
        ByteCode &operator=(const ByteCode &)=delete;

        ByteCode *connect(ByteCode *new_code);
        bool write_v1(FILE *f, bool debug=false) const;
        ByteCode *read_v1(FILE *f, bool debug=false, int8_t read_type=code_element, bool to_son=false);
        [[nodiscard]] std::string getMD5_v1() const;

#ifdef AFUN_DEBUG
        void display() const;
#endif

        [[nodiscard]] AFUN_INLINE CodeType getType() const;
        [[nodiscard]] AFUN_INLINE char getPrefix() const;

        [[nodiscard]] AFUN_INLINE const char *getElement() const;
        [[nodiscard]] AFUN_INLINE BlockType getBlockType() const;
        [[nodiscard]] AFUN_INLINE ByteCode *getSon() const;
        [[nodiscard]] AFUN_INLINE aFuntool::FileLine getFileLine() const;
        [[nodiscard]] AFUN_INLINE const aFuntool::FilePath &getFilePath() const;

        [[nodiscard]] AFUN_INLINE ByteCode *toNext() const;
        [[nodiscard]] AFUN_INLINE ByteCode *toPrev() const;
        [[nodiscard]] AFUN_INLINE ByteCode *toFather() const;

    private:
        CodeType type;
        char prefix=aFuntool::NUL;

        union CodeData {
            char *element;  // union 内不使用 std::string
            struct Block {  // NOLINT 不需要初始化
                BlockType block_type;
                ByteCode *son;
            } block;
            AFUN_INLINE CodeData();
        } data;

        ByteCode *father = nullptr;
        ByteCode *next = nullptr;
        ByteCode *prev = nullptr;

        Code &belong;
        aFuntool::FileLine line;

        ~ByteCode();  // 限制 ByteCode 只能建立在堆上
    };
}

#include "code.inline.h"

#endif //AFUN_CODE_H
