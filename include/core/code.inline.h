#ifndef AFUN_CODE_INLINE_H
#define AFUN_CODE_INLINE_H
#include "code.h"

namespace aFuncore {
    inline Code *Code::create(FileLine line, ConstFilePath file) {
        return new Code(line, file);
    }

    inline Code *Code::create(const std::string &element,
                    aFuntool::FileLine line, aFuntool::ConstFilePath file, char prefix) {
        return new Code(element, line, file, prefix);
    }

    inline Code *Code::create(BlockType block_type, Code *son,
                    aFuntool::FileLine line, aFuntool::ConstFilePath file, char prefix) {
        return new Code(block_type, son, line, file);
    }

    inline CodeType Code::getType() const {
        return type;
    }

    inline char Code::getPrefix() const {
        return prefix;
    }

    inline const char *Code::getElement() const {
        if (type != code_element)
            return "";
        return element;
    }

    inline BlockType Code::getBlockType() const {
        if (type != code_block)
            return block_p;
        return block_type;
    }

    inline Code *Code::getSon() const {
        if (type != code_block)
            return nullptr;
        return son;
    }

    inline Code *Code::toNext() const {
        return next;
    }

    inline Code *Code::toPrev() const {
        return prev;
    }

    inline Code *Code::toFather() const {
        return father;
    }

    inline aFuntool::FileLine Code::getFileLine() const {
        return line;
    }

    inline aFuntool::FilePath Code::getFilePath() const {
        return file;
    }
}

#endif //AFUN_CODE_INLINE_H
