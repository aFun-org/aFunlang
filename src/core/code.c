/*
 * 文件名: code.c
 * 目标: 管理Code结构体的函数
 */

#include <stdio.h>
#include "aFun.h"
#include "tool.h"
#include "__code.h"

static af_Code *makeCode(char prefix, FileLine line, FilePath path) {
    af_Code *bt = calloc(1, sizeof(af_Code));
    bt->line = line;
    bt->prefix = prefix;
    if (path != NULL)
        bt->path = pathCopy(path);
    return bt;
}

af_Code *makeLiteralCode(char *literal_data, char *func, char prefix, FileLine line, FilePath path) {
    af_Code *bt = makeCode(prefix, line, path);
    bt->type = literal;
    bt->literal.literal_data = strCopy(literal_data);
    bt->literal.func = strCopy(func);
    return bt;
}


af_Code *makeVariableCode(char *var, char prefix, FileLine line, FilePath path) {
    af_Code *bt = makeCode(prefix, line, path);
    bt->type = variable;
    bt->variable.name = strCopy(var);
    return bt;
}

/*
 * 函数名: countElement
 * 目标: 统计元素个数（不包括元素的子元素）
 */
static bool countElement(af_Code *element, CodeUint *elements, CodeUint *count, af_Code **next) {
    CodeUint to_next = 0;  // 表示紧接着的元素都不纳入统计(指block的子元素)

    for (*elements = 0; element != NULL; *next = element, element = element->next) {
        (*count)++;
        if (to_next == 0)
            (*elements)++;
        else
            to_next--;

        if (element->type == block)
            to_next += element->block.elements;
    }

    if (to_next != 0 || *elements == 0)  // elements不允许为0
        return false;
    return true;
}

af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix, FileLine line, FilePath path, af_Code **next) {
    af_Code *bt = NULL;
    af_Code *tmp = NULL;
    CodeUint elements = 0;
    CodeUint count = 0;

    if (next == NULL)
        next = &tmp;

    if (!countElement(element, &elements, &count, next))
        return NULL;

    bt = makeCode(prefix, line, path);
    bt->type = block;
    bt->block.type = type;
    bt->block.elements = elements;
    bt->block.count = count;
    bt->next = element;
    return bt;
}

af_Code *connectCode(af_Code **base, af_Code *next) {
    while ((*base) != NULL)
        base = &((*base)->next);
    *base = next;

    while (next != NULL && next->next != NULL)
        next = next->next;

    return next;
}

af_Code *copyCode(af_Code *base, FilePath *path) {
    af_Code *dest = NULL;
    af_Code **pdest = &dest;

    for (NULL; base != NULL; base = base->next) {
        *pdest = makeCode(base->prefix, base->line, base->path);
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
                (*pdest)->block.count = base->block.count;
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

af_Code *freeCode(af_Code *bt) {
    if (bt == NULL)
        return NULL;

    af_Code *next = bt->next;
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

bool freeCodeWithElement(af_Code *bt, af_Code **next) {
    CodeUint count = 1 + bt->block.count;  // 要释放的元素个数
    for (NULL; count != 0; count--) {
        if (bt == NULL)
            return false;
        bt = freeCode(bt);
    }
    *next = bt;
    return true;
}

void freeAllCode(af_Code *bt) {
    while (bt != NULL)
        bt = freeCode(bt);
}

bool getCodeBlockNext(af_Code *bt, af_Code **next) {
    CodeUint count = 1 + bt->block.count;
    for (NULL; count != 0; count--, bt = bt->next) {
        if (bt == NULL)
            return false;
    }
    *next = bt;
    return true;
}

#define Done(write) do{if(!(write)){return false;}}while(0)

static bool writeCode(af_Code *bt, FILE *file) {
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
            Done(byteWriteUint_32(file, bt->block.count));
            break;
        default:
            break;
    }
    return true;
}

/*
 * 函数名: writeAllCode
 * 目标: 将Code写入字节码文件中
 * 备注: 写入字节码时不做语义检查, 在读取时最语义检查即可
 */
bool writeAllCode(af_Code *bt, FILE *file) {
    uint32_t count = 0;

    if (bt == NULL || bt->path == NULL)
        return false;

    for (af_Code *tmp = bt; tmp != NULL; tmp = tmp->next)  // 统计个数
        count++;

    Done(byteWriteUint_32(file,count));
    for (NULL; bt != NULL; bt = bt->next) {
        if (!writeCode(bt, file))
            return false;
    }

    return true;
}

static bool readCode(af_Code **bt, FILE *file) {
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

    *bt = makeCode((char)prefix, line, path);
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
            uint32_t count;
            Done(byteReadUint_8(file, &block_type));
            Done(byteReadUint_32(file,&elements));
            Done(byteReadUint_32(file,&count));
            (*bt)->block.type = block_type;
            (*bt)->block.elements = elements;
            (*bt)->block.elements = count;
            break;
        }
        default:
            break;
    }
    return true;
}

bool readAllCode(af_Code **bt, FILE *file) {
    uint32_t count;
    Done(byteReadUint_32(file,&count));

    for (NULL; count != 0; count--, bt = &((*bt)->next)) {
        if(!readCode(bt, file))
            return false;
    }
    return true;
}

void printCode(af_Code *bt) {
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