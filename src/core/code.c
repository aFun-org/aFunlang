/*
 * 文件名: code.c
 * 目标: 管理Code结构体的函数
 */

#include <stdio.h>
#include <ctype.h>
#include "aFunCore.h"
#include "tool.h"
#include "__code.h"

/* Code 创建函数 */
static af_Code *makeCode(char prefix, FileLine line, FilePath path);
static af_Code *freeCode(af_Code *bt);

/* Code 相关操作 */
static int countElement(af_Code *element, CodeUInt *elements, af_Code **next);

/* Code IO函数 */
static bool readCode(af_Code **bt, FILE *file);
static bool writeCode(af_Code *bt, FILE *file);

/* Code 转换STR函数 */
struct af_BlockEnd {
    char ch;
    struct af_BlockEnd *next;
};
static bool checkElementData(char *data);
static char *codeToStr_(af_Code *code, LayerInt *layer, struct af_BlockEnd **bn);
static char *codeEndToStr(CodeUInt code_end, LayerInt *layer, struct af_BlockEnd **bn);

static af_Code *makeCode(char prefix, FileLine line, FilePath path) {
    af_Code *bt = calloc(1, sizeof(af_Code));
    bt->line = line;
    bt->prefix = prefix;
    if (path != NULL)
        bt->path = pathCopy(path);
    return bt;
}

af_Code *makeElementCode(char *var, char prefix, FileLine line, FilePath path) {
    if (prefix != NUL && strchr(E_PREFIX, prefix) == NULL)
        prefix = NUL;

    af_Code *bt = makeCode(prefix, line, path);
    bt->type = code_element;
    bt->element.data = strCopy(var);
    return bt;
}

/* 判断该code是否令layer需要加1 */
/* 即判定是否有子元素 */
#define LAYER_ADD1(code) ((code)->type == code_block && !(code)->block.is_empty)

/*
 * 函数名: countElement
 * 目标: 统计元素个数（不包括元素的子元素
 * 返回1 表示没有跳出过多层级
 * 返回2 表示跳出了过多层级
 * 返回0 表示错误
 */
static int countElement(af_Code *element, CodeUInt *elements, af_Code **next) {
    CodeUInt layer = 0;  // layer 是相对于当前element的层级数, 可能为负数
    af_Code *tmp = NULL;
    if (next == NULL)
        next = &tmp;

    for (*elements = 0; element != NULL; *next = element, element = element->next) {
        if (layer == 0)
            (*elements)++;

        if (LAYER_ADD1(element))
            layer++;

        if (layer - element->code_end < 0)
            return 2;
        else
            layer = layer - element->code_end;
    }

    if (layer == 0)
        return 1;
    else  // 大于0, 出现错误
        return 0;
}

af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix, FileLine line, FilePath path, af_Code **next) {
    af_Code *bt = NULL;
    af_Code *tmp = NULL;  // 保存最后一个code的地址
    CodeUInt elements = 0;

    if (next == NULL)
        next = &tmp;

    if (prefix != NUL && strchr(B_PREFIX, prefix) == NULL)
        prefix = NUL;

    if (countElement(element, &elements, next) != 1)
        return NULL;

    bt = makeCode(prefix, line, path);
    bt->type = code_block;
    bt->block.type = type;
    bt->next = element;
    if (elements == 0)
        bt->block.is_empty = true;
    else
        (*next)->code_end++;  // 若 elements 不为 0, 则next指向最后一个元素
    return bt;
}

af_Code *pushCode(af_Code **base, af_Code *next) {
    while ((*base) != NULL)
        base = &((*base)->next);
    *base = next;

    while (next != NULL && next->next != NULL)
        next = next->next;

    return next;
}

af_Code *copyAllCode(af_Code *base, FilePath *path) {
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
                (*pdest)->block.is_empty = base->block.is_empty;
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

/*
 * 函数名: copyCode
 * 目标: 拷贝 code, 并为末尾的code设置合适的code_end
 */
af_Code *copyCode(af_Code *base, FilePath *path) {
    af_Code *dest = NULL;
    af_Code **pdest = &dest;

    CodeUInt layer = 0;
    for (NULL; base != NULL; base = base->next) {
        *pdest = makeCode(base->prefix, base->line, base->path);
        (*pdest)->type = base->type;
        switch (base->type) {
            case code_element:
                (*pdest)->element.data = strCopy(base->element.data);
                break;
            case code_block:
                (*pdest)->block.is_empty = base->block.is_empty;
                (*pdest)->block.type = base->block.type;
                break;
            default:
                break;
        }

        if (LAYER_ADD1(base))
            layer++;

        if ((layer - base->code_end) < 0) {  // base跳出layer, pdest不能按照base的code_end来, pdest只能刚好跳出layer
            (*pdest)->code_end = layer;
            break;
        } else {  // 仍没跳出 layer
            (*pdest)->code_end = base->code_end;
            layer -= base->code_end;
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

af_Code *getCodeNext(af_Code *bt) {
    if (!LAYER_ADD1(bt))
        return bt->next;

    CodeUInt layer = 1;
    bt = bt->next;  // 跳过第一个code_block
    while (bt != NULL) {
        if (LAYER_ADD1(bt))
            layer++;
        if (layer - bt->code_end == 0)  // layer为0表示迭代到该block的最后一个元素, 则该元素的下一个元素为该block的Next
            return bt->next;
        else if (layer - bt->code_end < 0)  // 当layer小于0时, 认为已经无next, 即连续跳出了多层
            return NULL;
        layer = layer - bt->code_end;
        bt = bt->next;
    }

    return NULL;
}

af_Code *getCodeElement(af_Code *bt) {
    if (!LAYER_ADD1(bt))
        return NULL;
    return bt->next;
}

#define Done(write) do{if(!(write)){return false;}}while(0)
static bool writeCode(af_Code *bt, FILE *file) {
    Done(byteWriteUint_8(file, bt->type));
    Done(byteWriteUint_8(file, bt->prefix));
    Done(byteWriteUint_32(file, bt->line));
    Done(byteWriteUint_32(file, bt->code_end));

    switch (bt->type) {
        case code_element:
            Done(byteWriteStr(file, bt->element.data));
            break;
        case code_block:
            Done(byteWriteUint_8(file, bt->block.type));
            Done(byteWriteUint_8(file, bt->block.is_empty));
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
bool writeAllCode(af_Code *bt, FilePath path) {
    if (bt == NULL || bt->path == NULL)
        return false;

    FILE *file = fopen(path, "wb");
    if (file == NULL)
        return false;

    for (NULL; bt != NULL; bt = bt->next) {
        if (!writeCode(bt, file))
            goto RETURN_FALSE;
        if (ferror(stdin))
            goto RETURN_FALSE;

        Done(byteWriteUint_8(file, (bt->next == NULL)));  // 记录是否为最后一位
    }

    fclose(file);
    return true;

RETURN_FALSE:
    fclose(file);
    return false;
}

static bool readCode(af_Code **bt, FILE *file) {
    uint8_t type;
    uint8_t prefix;
    uint32_t line;
    uint32_t code_end;

    Done(byteReadUint_8(file, &type));
    Done(byteReadUint_8(file, &prefix));
    Done(byteReadUint_32(file,&line));
    Done(byteReadUint_32(file, &code_end));

    *bt = makeCode((char)prefix, line, NULL);
    (*bt)->type = type;
    (*bt)->code_end = (CodeUInt)code_end;

    switch (type) {
        case code_element:
            Done(byteReadStr(file, &((*bt)->element.data)));
            break;
        case code_block: {
            uint8_t block_type;
            uint8_t is_empty;
            Done(byteReadUint_8(file, &block_type));
            Done(byteReadUint_8(file,&is_empty));
            (*bt)->block.type = block_type;
            (*bt)->block.is_empty = (CodeUInt)is_empty;
            break;
        }
        default:
            break;
    }
    return true;
}

bool readAllCode(af_Code **bt, FilePath path) {
    af_Code **base = bt;
    *bt = NULL;

    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return false;

    for (NULL; true;bt = &((*bt)->next)) {
        if(!readCode(bt, file))
            goto RETURN_FALSE;
        if (ferror(stdin))
            goto RETURN_FALSE;

        uint8_t last;
        Done(byteReadUint_8(file, &last));
        if (last)
            break;
    }

    if (*base != NULL)
        (*base)->path = strCopy(path);
    fclose(file);
    return true;

RETURN_FALSE:
    fclose(file);
    return false;
}

/*
 * 函数名: checkElementData
 * 目标: 检查element中data字符串是否有空白符
 */
static bool checkElementData(char *data) {
    for (char *ch = data; *ch != NUL; ch++) {
        if (isspace(*ch) || *ch == '\n')
            return true;
    }
    return false;
}

/*
 * 函数名: codeEndToStr
 * 目标: 转换element或开括号为字符串
 * 若遇到开括号则设定bn, 并且设定layer
 *
 * bn中记录了开括号的顺序, 再打印闭括号时将按该顺序打印
 */
static char *codeToStr_(af_Code *code, LayerInt *layer, struct af_BlockEnd **bn) {
    char *re = charToStr(code->prefix);
    if (code->type == code_element) {
        if (checkElementData(code->element.data)) {  // 需要|xx xx|语法
            re = strJoin(re, "|", true, false);
            re = strJoin(re, code->element.data, true, false);
            re = strJoin(re, "| ", true, false);
        } else
            re = strJoin(re, code->element.data, true, false);
    } else if (code->block.is_empty) {
        switch(code->block.type) {
            case parentheses:
                re = strJoin(re, "()", true, false);
                break;
            case brackets:
                re = strJoin(re, "[]", true, false);
                break;
            case curly:
                re = strJoin(re, "{}", true, false);
                break;
            default:
                break;
        }
    } else {
        char ch = NUL;
        switch(code->block.type) {
            case parentheses:
                re = strJoin(re, "(", true, false);
                ch = ')';
                break;
            case brackets:
                re = strJoin(re, "[", true, false);
                ch = ']';
                break;
            case curly:
                re = strJoin(re, "{", true, false);
                ch = '}';
                break;
            default:
                break;
        }
        struct af_BlockEnd *new = calloc(1, sizeof(struct af_BlockEnd));
        new->ch = ch;
        new->next = *bn;
        *bn = new;
        (*layer)++;
    }
    return re;
}

/*
 * 函数名: codeEndToStr
 * 目标: 转换闭括号为字符串
 */
static char *codeEndToStr(CodeUInt code_end, LayerInt *layer, struct af_BlockEnd **bn) {
    char *re = NEW_STR(code_end);
    for (size_t i = 0; code_end > 0; code_end--, i++) {
        if (*bn == NULL)
            break;
        (*layer)--;
        re[i] = (*bn)->ch;

        struct af_BlockEnd *tmp = (*bn)->next;
        free(*bn);
        *bn = tmp;
    }
    re = strJoin(re, " ", true, false);
    return re;
}

/*
 * 函数名: codeToStr
 * 目标: 转换n个元素(或n个函数调用)为code
 */
char *codeToStr(af_Code *code, int n) {
    char *re = strCopy(NULL);
    struct af_BlockEnd *bn = NULL;
    LayerInt layer = 0;

    for (NULL; code != NULL && layer >= 0 && (n > 0 || n == -1); code = code->next) {
        if (strlen(re) >= CODE_STR_MAX_SIZE) {
            re = strJoin(re, " ...", true, false);  // 限度
            break;
        }

        re = strJoin(re, codeToStr_(code, &layer, &bn), true, true);
        if (code->code_end != 0)
            re = strJoin(re, codeEndToStr(code->code_end, &layer, &bn), true, true);
        if (n != -1 && layer == 0) /* 完成一个元素的打印 */
            n--;
    }
    return re;
}

static void printLayerSpace(size_t layer) {
    for (size_t i = 0; i < layer; i++)
        printf("    ");
}

void printCode(af_Code *bt) {
    size_t layer = 0;
    for (NULL; bt != NULL || layer < 0; bt = bt->next) {
        printLayerSpace(layer);
        layer = layer - bt->code_end;
        switch (bt->type) {
            case code_element:
                printf("element: [prefix (%c)] [end %u] [data '%s']\n", bt->prefix, bt->code_end, bt->element.data);
                break;
            case code_block:
                if (LAYER_ADD1(bt))
                    layer++;
                printf("code: [prefix (%c)] [end %u] [type %c] [empty %d]\n", bt->prefix, bt->code_end, bt->block.type, bt->block.is_empty);
                break;
            default:
                printf("Unknown: [prefix (%c)] [end %u] [type %d]\n", bt->prefix, bt->code_end, bt->type);
                break;
        }
    }
}

enum af_CodeType getCodeType(af_Code *code) {
    return code->type;
}

enum af_BlockType getCodeBlockType(af_Code *code) {
    if (code->type != code_block)
        return '(';
    return code->block.type;
}

char getCodePrefix(af_Code *code) {
    return code->prefix;
}

CodeUInt getCodeEndCount(af_Code *code) {
    return code->code_end;
}

char *getCodeElementData(af_Code *code) {
    if (code->type != code_element)
        return NULL;
    return code->element.data;
}

CodeUInt getCodeElementCount(af_Code *code) {
    if (!LAYER_ADD1(code))
        return 0;

    CodeUInt count;
    if (countElement(code->next, &count, NULL) == 0)
        return 0;
    return count;
}