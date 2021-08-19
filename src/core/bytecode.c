/*
 * 文件名: bytecode.c
 * 目标: 管理ByteCode结构体的函数
 */

#include <stdio.h>
#include "aFun.h"
#include "__bytecode.h"
#include "tool.h"

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
    af_ByteCode *tmp = NULL;
    ByteCodeUint count = 0;

    if (next == NULL)
        next = &tmp;

    if (!countElement(element, &count, next))
        return NULL;

    bt = makeByteCode(prefix, line, path);
    bt->type = block;
    bt->block.type = type;
    bt->block.elements = count;
    bt->next = element;
    return bt;
}

af_ByteCode *connectByteCode(af_ByteCode **base, af_ByteCode *next) {
    while ((*base) != NULL)
        base = &((*base)->next);
    *base = next;

    while (next != NULL && next->next != NULL)
        next = next->next;

    return next;
}

af_ByteCode *copyByteCode(af_ByteCode *base, FilePath *path) {
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
    free(bt->path);
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

    free(bt);
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

#define Done(write) do{if(!(write)){return false;}}while(0)

static bool writeByteCode(af_ByteCode *bt, FILE *file) {
    Done(byteWriteUint_8(file, bt->type));
    Done(byteWriteUint_8(file, bt->prefix));
    Done(byteWriteUint_32(file, bt->line));

    if (bt->path != NULL) {
        Done(byteWriteUint_8(file, true));  // 表示有path
        Done(byteWriteStr(file, bt->path));
    } else {
        Done(byteWriteUint_8(file, false));  // 表示无path
    }

    switch (bt->type) {
        case literal:
            Done(byteWriteStr(file, bt->literal.literal_data));
            Done(byteWriteStr(file, bt->literal.func));
            break;
        case variable:
            Done(byteWriteStr(file, bt->variable.name));
            break;
        case block:
            Done(byteWriteUint_8(file, bt->block.type));
            Done(byteWriteUint_32(file, bt->block.elements));
            break;
        default:
            break;
    }
    return true;
}

/*
 * 函数名: writeAllByteCode
 * 目标: 将ByteCode写入字节码文件中
 * 备注: 写入字节码时不做语义检查, 在读取时最语义检查即可
 */
bool writeAllByteCode(af_ByteCode *bt, FILE *file) {
    uint32_t count = 0;

    if (bt == NULL || bt->path == NULL)
        return false;

    for (af_ByteCode *tmp = bt; tmp != NULL; tmp = tmp->next)  // 统计个数
        count++;

    Done(byteWriteUint_32(file,count));
    for (NULL; bt != NULL; bt = bt->next) {
        if (!writeByteCode(bt, file))
            return false;
    }

    return true;
}

static bool readByteCode(af_ByteCode **bt, FILE *file) {
    uint8_t type;
    uint8_t prefix;
    uint32_t line;
    uint8_t have_path;
    char *path = NULL;

    Done(byteReadUint_8(file, &type));
    Done(byteReadUint_8(file, &prefix));
    Done(byteReadUint_32(file,&line));
    Done(byteReadUint_8(file, &(have_path)));

    if (have_path)
        Done(byteReadStr(file, &path));

    *bt = makeByteCode((char)prefix, line, path);
    free(path);
    (*bt)->type = type;

    switch (type) {
        case literal:
            Done(byteReadStr(file, &((*bt)->literal.literal_data)));
            Done(byteReadStr(file, &((*bt)->literal.func)));
            break;
        case variable:
            Done(byteReadStr(file, &((*bt)->variable.name)));
            break;
        case block: {
            uint8_t block_type;
            uint32_t elements;
            Done(byteReadUint_8(file, &block_type));
            Done(byteReadUint_32(file,&elements));
            (*bt)->block.type = block_type;
            (*bt)->block.elements = elements;
            break;
        }
        default:
            break;
    }
    return true;
}

/*
 * 函数名: writeAllByteCode
 * 目标: 将ByteCode写入字节码文件中
 * 备注: 写入字节码时不做语义检查, 在读取时最语义检查即可 【语义检查还未实现】
 */
bool readAllByteCode(af_ByteCode **bt, FILE *file) {
    uint32_t count;
    Done(byteReadUint_32(file,&count));

    for (NULL; count != 0; count--, bt = &((*bt)->next)) {
        if(!readByteCode(bt, file))
            return false;
    }
    return true;
}

void printByteCode(af_ByteCode *bt) {
    for (NULL; bt != NULL; bt = bt->next) {
        switch (bt->type) {
            case literal:
                printf("literal: %s %s prefix: %d\n", bt->literal.literal_data, bt->literal.func, bt->prefix);
                break;
            case variable:
                printf("variable: %s prefix: %d\n", bt->variable.name, bt->prefix);
                break;
            case block:
                printf("variable: %d %d prefix: %d\n", bt->block.elements, bt->block.type, bt->prefix);
                break;
            default:
                printf("Unknow: %d prefix: %d\n", bt->type, bt->prefix);
                break;
        }
    }
}