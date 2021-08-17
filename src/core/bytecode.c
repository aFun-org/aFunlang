/*
 * 文件名: bytecode.c
 * 目标: 管理ByteCode结构体的函数
 */

#include "aFun.h"
#include "bytecode.h"

static af_ByteCode *makeByteCode(char prefix, FileLine line, FilePath path) {
    af_ByteCode *bt = calloc(1, sizeof(af_ByteCode));
    bt->line = line;
    bt->prefix = prefix;
    if (path != NULL)
        bt->path = pathCopy(path);
    return bt;
}

af_ByteCode *makeLiteralByteCode(char *literal_data, char *func, char prefix, FileLine line, FilePath path) {
    af_ByteCode *bt = makeByteCode(prefix, line, path);
    bt->type = literal;
    bt->literal.literal_data = strCopy(literal_data);
    bt->literal.func = strCopy(func);
    return bt;
}


af_ByteCode *makeVariableByteCode(char *var, char prefix, FileLine line, FilePath path) {
    af_ByteCode *bt = makeByteCode(prefix, line, path);
    bt->type = variable;
    bt->variable.name = strCopy(var);
    return bt;
}

/*
 * 函数名: countElement
 * 目标: 统计元素个数（不包括元素的子元素）
 */
static bool countElement(af_ByteCode *element, ByteCodeUint *count, af_ByteCode **next) {
    ByteCodeUint to_next = 0;  // 表示紧接着的元素都不纳入统计(指block的子元素)

    for (*count = 0; element != NULL; *next = element, element = element->next) {
        if (to_next == 0)
            (*count)++;
        else
            to_next--;

        if (element->type == block)
            to_next += element->block.elements;
    }

    if (to_next != 0)
        return false;
    return true;
}

af_ByteCode *makeBlockByteCode(enum af_BlockType type, af_ByteCode *element, char prefix, FileLine line, FilePath path, af_ByteCode **next) {
    af_ByteCode *bt = NULL;
    ByteCodeUint count = 0;

    if (!countElement(element, &count, next))
        return NULL;

    bt = makeByteCode(prefix, line, path);
    bt->type = block;
    bt->block.type = type;
    bt->block.elements = count;
    bt->next = element;
    return bt;
}

af_ByteCode *CopyByteCode(af_ByteCode *base, FilePath *path) {
    af_ByteCode *dest = NULL;
    af_ByteCode **pdest = &dest;

    for (NULL; base != NULL; base = base->next) {
        *pdest = makeByteCode(base->prefix, base->line, base->path);
        (*pdest)->type = base->type;
        switch (base->type) {
            case literal:
                (*pdest)->literal.literal_data = strCopy(base->literal.literal_data);
                (*pdest)->literal.func = strCopy(base->literal.func);
                break;

            case variable:
                (*pdest)->variable.name = strCopy(base->variable.name);
                break;

            case block:
                (*pdest)->block.elements = base->block.elements;
                (*pdest)->block.type = base->block.type;
                break;

            default:
                break;
        }
    }

    if (dest != NULL && path != NULL) {
        free(dest->path);
        dest->path = pathCopy(path);
    }

    return dest;
}

af_ByteCode *freeByteCode(af_ByteCode *bt) {
    if (bt == NULL)
        return NULL;

    af_ByteCode *next = bt->next;
    switch (bt->type) {
        case literal:
            free(bt->literal.literal_data);
            free(bt->literal.func);
            break;

        case variable:
            free(bt->variable.name);
            break;

        default:
            break;
    }

    return next;
}

bool freeByteCodeWithElement(af_ByteCode *bt, af_ByteCode **next) {
    ByteCodeUint count = 1;  // 要释放的元素个数
    for (NULL; count != 0; count--) {
        if (bt == NULL)
            return false;
        if (bt->type == block)
            count += bt->block.elements;
        bt = freeByteCode(bt);
    }

    *next = bt;
    return true;
}

void freeAllByteCode(af_ByteCode *bt) {
    while (bt != NULL)
        bt = freeByteCode(bt);
}
