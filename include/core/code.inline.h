#ifndef AFUN_CODE_INLINE_H
#define AFUN_CODE_INLINE_H
#include "code.h"

namespace aFuncore {
    Code::Code(aFuntool::FilePath file_) : code{new ByteCode(*this, 0)}, file{std::move(file_)} {

    }

    Code::ByteCode *Code::getByteCode() const{
        return code;
    }

    const aFuntool::FilePath &Code::getFilePath() const{
        return file;
    }

    Code::ByteCode::CodeType Code::ByteCode::getType() const {
        return type;
    }

    char Code::ByteCode::getPrefix() const {
        return prefix;
    }

    const char *Code::ByteCode::getElement() const {
        if (type != code_element)
            return "";
        return data.element;
    }

    Code::ByteCode::BlockType Code::ByteCode::getBlockType() const {
        if (type != code_block)
            return block_p;
        return data.block.block_type;
    }

    Code::ByteCode *Code::ByteCode::getSon() const {
        if (type != code_block)
            return nullptr;
        return data.block.son;
    }

    Code::ByteCode *Code::ByteCode::toNext() const {
        return next;
    }

    Code::ByteCode *Code::ByteCode::toPrev() const {
        return prev;
    }

    Code::ByteCode *Code::ByteCode::toFather() const {
        return father;
    }

    aFuntool::FileLine Code::ByteCode::getFileLine() const {
        return line;
    }

    const aFuntool::FilePath &Code::ByteCode::getFilePath() const{
        return belong.getFilePath();
    }

    Code::ByteCode::CodeData::CodeData() : element{nullptr} {

    }
}

#endif //AFUN_CODE_INLINE_H
