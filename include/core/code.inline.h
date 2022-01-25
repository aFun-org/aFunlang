#ifndef AFUN_CODE_INLINE_H
#define AFUN_CODE_INLINE_H
#include "code.h"

namespace aFuncore {
    inline Code::Code(aFuntool::FilePath file_) : code{new ByteCode(*this, 0)}, file{std::move(file_)} {

    }

    inline Code::ByteCode *Code::getByteCode() const{
        return code;
    }

    inline const aFuntool::FilePath &Code::getFilePath() const{
        return file;
    }

    inline Code::ByteCode::CodeType Code::ByteCode::getType() const {
        return type;
    }

    inline char Code::ByteCode::getPrefix() const {
        return prefix;
    }

    inline const char *Code::ByteCode::getElement() const {
        if (type != code_element)
            return "";
        return data.element;
    }

    inline Code::ByteCode::BlockType Code::ByteCode::getBlockType() const {
        if (type != code_block)
            return block_p;
        return data.block_type;
    }

    inline Code::ByteCode *Code::ByteCode::getSon() const {
        if (type != code_block)
            return nullptr;
        return data.son;
    }

    inline Code::ByteCode *Code::ByteCode::toNext() const {
        return next;
    }

    inline Code::ByteCode *Code::ByteCode::toPrev() const {
        return prev;
    }

    inline Code::ByteCode *Code::ByteCode::toFather() const {
        return father;
    }

    inline aFuntool::FileLine Code::ByteCode::getFileLine() const {
        return line;
    }

    inline const aFuntool::FilePath &Code::ByteCode::getFilePath() const{
        return belong.getFilePath();
    }

    inline Code::ByteCode::CodeData::CodeData() : element{nullptr} {

    }
}

#endif //AFUN_CODE_INLINE_H
