/*
 * 文件名: code.c
 * 目标: 管理Code结构体的函数
 */

#include <stdio.h>
#include "aFun.h"
#include "tool.h"
#include "__code.h"

/* Code 创建函数 */
static af_Code *makeCode(char prefix, FileLine line, FilePath path);
static af_Code *freeCode(af_Code *bt);

/* Code 操作函数 */
static void countElement(af_Code *element, CodeUint *elements, af_Code **next);

/* Code IO函数 */
static bool readCode(af_Code **bt, FILE *file);
static bool writeCode(af_Code *bt, FILE *file);

static af_Code *makeCode(char prefix, FileLine line, FilePath path) {
    af_Code *bt = calloc(1, sizeof(af_Code));
    bt->line = line;
    bt->prefix = prefix;
    if (path != NULL)
        bt->path = pathCopy(path);
    return bt;
}

af_Code *makeElementCode(char *var, char prefix, FileLine line, FilePath path) {
    if (prefix != NUL && strchr(LV_PREFIX, prefix) == NULL)
        prefix = NUL;

    af_Code *bt = makeCode(prefix, line, path);
    bt->type = code_element;
    bt->element.data = strCopy(var);
    return bt;
}

/*
 * 函数名: countElement
 * 目标: 统计元素个数（不包括元素的子元素）
 */
static void countElement(af_Code *element, CodeUint *elements, af_Code **next) {
    CodeUint layer = 0;

    for (*elements = 0; element != NULL; *next = element, element = element->next) {
        if (layer == 0)
            (*elements)++;

        if (element->type == code_block)
            layer++;
        if (element->code_end)
            layer = layer - element->code_end;
    }
}

af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix, FileLine line, FilePath path, af_Code **next) {
    af_Code *bt = NULL;
    af_Code *tmp = NULL;  // 保存最后一个code的地址
    CodeUint elements = 0;

    if (next == NULL)
        next = &tmp;

    if (prefix != NUL && strchr(B_PREFIX, prefix) == NULL)
        prefix = NUL;

    countElement(element, &elements, next);
    bt = makeCode(prefix, line, path);
    bt->type = code_block;
    bt->block.type = type;
    bt->block.elements = elements;
    bt->next = element;
    if (*next != NULL)
        (*next)->code_end++;
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
        (*pdest)->code_end = base->code_end;
        switch (base->type) {
            case code_element:
                (*pdest)->element.data = strCopy(base->element.data);
                break;
            case code_block:
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

static af_Code *freeCode(af_Code *bt) {
    af_Code *next = bt->next;
    free(bt->path);
    switch (bt->type) {
        case code_element:
            free(bt->element.data);
            break;
        default:
            break;
    }

    free(bt);
    return next;
}

void freeAllCode(af_Code *bt) {
    while (bt != NULL)
        bt = freeCode(bt);
}

bool getCodeBlockNext(af_Code *bt, af_Code **next) {
    if (bt->block.elements == 0) {
        *next = bt->next;
        return true;
    }

    CodeUint count = 1;
    bt = bt->next;
    for (NULL; count != 0; bt = bt->next) {
        if (bt == NULL)
            return false;
        if (bt->type == code_block)
            count++;
        count = count - bt->code_end;
    }
    *next = bt;
    return true;
}

#define Done(write) do{if(!(write)){return false;}}while(0)
static bool writeCode(af_Code *bt, FILE *file) {
    Done(byteWriteUint_8(file, bt->type));
    Done(byteWriteUint_8(file, bt->prefix));
    Done(byteWriteUint_32(file, bt->line));
    Done(byteWriteUint_32(file, bt->code_end));

    if (bt->path != NULL) {
        Done(byteWriteUint_8(file, true));  // 表示有path
        Done(byteWriteStr(file, bt->path));
    } else {
        Done(byteWriteUint_8(file, false));  // 表示无path
    }

    switch (bt->type) {
        case code_element:
            Done(byteWriteStr(file, bt->element.data));
            break;
        case code_block:
            Done(byteWriteUint_8(file, bt->block.type));
            Done(byteWriteUint_32(file, bt->block.elements));
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
    uint32_t code_end;
    char *path = NULL;

    Done(byteReadUint_8(file, &type));
    Done(byteReadUint_8(file, &prefix));
    Done(byteReadUint_32(file,&line));
    Done(byteReadUint_8(file, &(have_path)));
    if (have_path)
        Done(byteReadStr(file, &path));
    Done(byteReadUint_32(file, &code_end));

    *bt = makeCode((char)prefix, line, path);
    free(path);
    (*bt)->type = type;
    (*bt)->code_end = code_end;

    switch (type) {
        case code_element:
            Done(byteReadStr(file, &((*bt)->element.data)));
            break;
        case code_block: {
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
            case code_element:
                printf("code_element: %s prefix: %d\n", bt->element.data, bt->prefix);
                break;
            case code_block:
                printf("code_block: %d %d prefix: %d\n", bt->block.elements, bt->block.type, bt->prefix);
                break;
            default:
                printf("Unknow: %d prefix: %d\n", bt->type, bt->prefix);
                break;
        }
    }
}