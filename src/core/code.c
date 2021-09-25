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
static bool countElement(af_Code *element, CodeInt *elements, af_Code **next);

/* Code IO函数 */
static bool readCode(af_Code **bt, FILE *file);
static bool writeCode(af_Code *bt, FILE *file);

/* Code 转换STR函数 */
struct af_BlockEnd {
    char ch;
    struct af_BlockEnd *next;
};
static bool checkElementData(char *data);
static char *codeToStr_(af_Code *code, CodeInt *layer, struct af_BlockEnd **bn);
static char *codeEndToStr(CodeInt code_end, CodeInt *layer, struct af_BlockEnd **bn);

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

/*
 * 函数名: countElement
 * 目标: 统计元素个数（不包括元素的子元素）
 */
static bool countElement(af_Code *element, CodeInt *elements, af_Code **next) {
    CodeInt layer = 0;

    for (*elements = 0; element != NULL; *next = element, element = element->next) {
        if (layer == 0)
            (*elements)++;
        if (layer < 0)
            return false;

        if (element->type == code_block)
            layer++;
        if (element->code_end)
            layer = layer - element->code_end;
    }
    return true;
}

af_Code *makeBlockCode(enum af_BlockType type, af_Code *element, char prefix, FileLine line, FilePath path, af_Code **next) {
    af_Code *bt = NULL;
    af_Code *tmp = NULL;  // 保存最后一个code的地址
    CodeInt elements = 0;

    if (next == NULL)
        next = &tmp;

    if (prefix != NUL && strchr(B_PREFIX, prefix) == NULL)
        prefix = NUL;

    if (!countElement(element, &elements, next))
        return NULL;

    bt = makeCode(prefix, line, path);
    bt->type = code_block;
    bt->block.type = type;
    bt->block.elements = elements;
    bt->next = element;
    if (*next != NULL)
        (*next)->code_end++;
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

af_Code *getCodeNext(af_Code *bt) {
    if (bt->type == code_element || bt->block.elements == 0) {
        return bt->next;
    }

    CodeInt layer = 1;
    bt = bt->next;  // 跳过第一个code_block
    while (layer > 0) {
        if (bt->type == code_block && bt->block.elements != 0)
            layer++;
        layer = layer - bt->code_end;
        bt = bt->next;
    }

    if (layer == 0)  // 当layer小于0时, 认为已经无next, 即连续跳出了多层
        return bt;
    return NULL;
}

af_Code *getCodeElement(af_Code *bt) {
    if (bt->type == code_element || bt->block.elements == 0)
        return NULL;
    return bt->next;
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
    Done(byteReadUint_32(file, &code_end));
    Done(byteReadUint_8(file, &(have_path)));
    if (have_path)
        Done(byteReadStr(file, &path));

    *bt = makeCode((char)prefix, line, path);
    free(path);
    (*bt)->type = type;
    (*bt)->code_end = (CodeInt)code_end;

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
            (*bt)->block.elements = (CodeInt)elements;
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
 */
static char *codeToStr_(af_Code *code, CodeInt *layer, struct af_BlockEnd **bn) {
    char *re = charToStr(code->prefix);
    if (code->type == code_element) {
        if (checkElementData(code->element.data)) {  // 需要|xx xx|语法
            re = strJoin(re, "|", true, false);
            re = strJoin(re, code->element.data, true, false);
            re = strJoin(re, "| ", true, false);
        } else
            re = strJoin(re, code->element.data, true, false);
    } else if (code->block.elements == 0) {
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
 * 目标: 转换收尾括号为字符串
 */
static char *codeEndToStr(CodeInt code_end, CodeInt *layer, struct af_BlockEnd **bn) {
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
    CodeInt layer = 0;

    for (NULL; code != NULL && layer >= 0 && (n > 0 || n == -1); code = code->next) {
        if (strlen(re) >= CODE_STR_MAX_SIZE) {
            re = strJoin(re, " ...", true, false);  // 限度
            break;
        }

        char *get = codeToStr_(code, &layer, &bn);
        re = strJoin(re, get, true, true);
        if (code->code_end != 0) {
            get = codeEndToStr(code->code_end, &layer, &bn);
            re = strJoin(re, get, true, true);
        }
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
                printf("element: [prefix (%c)] [end %ld] [data '%s']\n", bt->prefix, bt->code_end, bt->element.data);
                break;
            case code_block:
                layer++;
                printf("code: [prefix (%c)] [end %ld] [type %c] [elements %ld]\n", bt->prefix, bt->code_end, bt->block.type, bt->block.elements);
                break;
            default:
                printf("Unknown: [prefix (%c)] [end %ld] [type %d]\n", bt->prefix, bt->code_end, bt->type);
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

CodeInt getCodeEndCount(af_Code *code) {
    return code->code_end;
}

char *getCodeElementData(af_Code *code) {
    if (code->type != code_element)
        return NULL;
    return code->element.data;
}

CodeInt getCodeElementCount(af_Code *code) {
    if (code->type != code_block)
        return -1;
    return code->block.elements;
}