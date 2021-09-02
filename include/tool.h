/*
 * 文件名: tool.h
 * 目标: aFun tool公共API
 * aFunTool是aFun实用工具库, 内含aFun调用的实用函数
 */

#ifndef AFUN__TOOL_H
#define AFUN__TOOL_H
#include <wchar.h>
#include <string.h>
#include <signal.h>
#include "macro.h"
#ifdef _MSC_VER
#include "../src/deps/dlfcn/dlfcn.h"
#else
#include <dlfcn.h>
#endif

/* md5计算工具 */
#define READ_DATA_SIZE	(1024)
#define MD5_SIZE (16)
#define MD5_STR_LEN (MD5_SIZE * 2)
#define MD5_STRING (MD5_STR_LEN + 1)
int getFileMd5(const char *path, char *md5str);

/* 哈希工具 */
typedef long int time33_t;

time33_t time33(char *str);
time33_t w_time33(wchar_t *str);

/* 字符串工具 */
#define EQ_STR(str1, str2) (!strcmp((str1), (str2)))
#define EQ_WSTR(wid1, wid2) (!wcscmp((wid1), (wid2)))

#define pathCopy(path) ((FilePath)strCopy((char *)(path)))

#define NEW_STR(size) (char *)calloc((size) + 1, sizeof(char))
#define NEW_WSTR(size) (wchar_t *)calloc((size) + 1, sizeof(wchar_t))
#define STR_LEN(p) (((p) == NULL) ? 0 : strlen((p)))
#define WSTR_LEN(p) (((p) == NULL) ? 0 : wcslen((p)))
char *charToStr(char ch);
char *strCopy(const char *str);
wchar_t *wstrCopy(const wchar_t *str);
wchar_t *wstrWithWchar(wchar_t *str, size_t size, int free_old, ...);
wchar_t *wstrWithWchar_(wchar_t *str, wint_t new, bool free_old);
wchar_t *wstrExpansion(wchar_t *str, size_t size, bool free_old);
char *strJoinIter(char *base, int free_base, ...);
char *strJoin(char *first, char *second, bool free_first, bool free_last);
char *strJoin_(char *first, char *second, bool free_first, bool free_last);
wchar_t *wstrJoin(wchar_t *first, wchar_t *second, bool free_first, bool free_last);
wchar_t *wstrJoin_(wchar_t *first, wchar_t *second, bool free_first, bool free_last);
wchar_t *wstrCopySelf(wchar_t *str, long times);
wchar_t *wstrReverse(wchar_t *str);
wchar_t *convertToWstr(char *str, bool free_old);
char *convertToStr(wchar_t *wstr, bool free_old);

/* 路径工具 */
#ifdef __linux__

#define SEP "/"
#define SEP_CH '/'
#define SHARED_MARK ".so"

#else

#define SEP "\\"
#define SEP_CH '\\'
#define SHARED_MARK ".dll"

#endif

/* 文件处理工具 */
int checkFile(char *path);
char *getFileName(char *path_1);
char *fileNameToVar(char *name, bool need_free);
char *findPath(char *path, char *env, bool need_free);

/* 信号处理工具 */
typedef int vsignal;
typedef struct SignalTag SignalTag;
struct SignalTag{
    volatile vsignal signum;  // 信号
    volatile enum SignalType{
        signal_reset=0,  // 没有信号
        signal_appear,  // 信号未被处理
    } status;
};

extern volatile struct SignalTag signal_tag;  // 在tool.c中定义
void afSignalHandler(int signum);

/* 时间工具 */
void safeSleep(double ms);

/* 动态库工具(dlc): 处理动态库的使用 */

/*
 * NEW_DLC_SYMBOL: 用于定义指定类型的symbol结构体
 * DLC_SYMBOL: 指定类型的symbol结构体名
 * GET_SYMBOL: 访问symbol成员的值
 * MAKE_SYMBOL: 生成一个symbol
 * COPY_SYMBOL: 拷贝一个symbol(拷贝其引用)
 * READ_SYMBOL: 在dlc中获取一个symbol
 * FREE_SYMBOL： 释放symbol
 *
 * openLibary: 打开动态库
 * freeLibary: 释放动态库
 * dlcExit: 释放所有动态库
 */

#define DEFINE_DLC_SYMBOL(NAME) typedef struct DLC##NAME##SYMBOL *pDLC##NAME##SYMBOL
#define NEW_DLC_SYMBOL(TYPE, NAME) typedef struct DLC##NAME##SYMBOL { \
TYPE *symbol; \
struct DlcHandle *dlc; \
} DLC##NAME##SYMBOL, *pDLC##NAME##SYMBOL

#define DLC_SYMBOL(NAME) pDLC##NAME##SYMBOL
#define GET_SYMBOL(SYMBOL) (*((SYMBOL)->symbol))
#define MAKE_SYMBOL(symbol, TYPE) ((pDLC##TYPE##SYMBOL) (makeSymbol_(symbol)))
#define COPY_SYMBOL(ds, TYPE) ((pDLC##TYPE##SYMBOL) (copySymbol_((DlcSymbol_ *)(ds))))
#define READ_SYMBOL(dlc, name, TYPE) ((pDLC##TYPE##SYMBOL) (getSymbol_((dlc), (name))))
#define FREE_SYMBOL(symbol) ((symbol) != NULL ? (freeSymbol_((DlcSymbol_ *)(symbol)), NULL) : NULL)

typedef struct DlcSymbol_ DlcSymbol_;
typedef struct DlcHandle DlcHandle;

struct DlcHandle *openLibary(const char *file, int mode);
struct DlcSymbol_ *makeSymbol_(void *symbol);
struct DlcSymbol_ *copySymbol_(struct DlcSymbol_ *ds);
struct DlcSymbol_ *getSymbol_(struct DlcHandle *dlc, const char *name);
void freeSymbol_(struct DlcSymbol_ *symbol);
bool freeLibary(struct DlcHandle *dlc);
void dlcExit(void);

// byte工具
#define byteWriteInt_8(file, s) (byteWriteUint_8(file, ((uint8_t)(s))))
#define byteWriteInt_16(file, s) (byteWriteUint_16(file, ((uint16_t)(s))))
#define byteWriteInt_32(file, s) (byteWriteUint_32(file, ((uint32_t)(s))))
#define byteWriteInt_64(file, s) (byteWriteUint_64(file, ((uint64_t)(s))))

#define byteReadInt_8(file, s) (byteReadUint_8(file, ((uint8_t *)(s))))
#define byteReadInt_16(file, s) (byteReadUint_16(file, ((uint16_t *)(s))))
#define byteReadInt_32(file, s) (byteReadUint_32(file, ((uint32_t *)(s))))
#define byteReadInt_64(file, s) (byteReadUint_64(file, ((uint64_t *)(s))))

enum af_EndianType{
    little_endian = 0,
    big_endian
};

extern enum af_EndianType endian;

void getEndian();
bool byteWriteUint_8(FILE *file, uint8_t ch);
bool byteWriteUint_16(FILE *file, uint16_t num);
bool byteWriteUint_32(FILE *file, uint32_t num);
bool byteWriteUint_64(FILE *file, uint64_t num);
bool byteWriteStr(FILE *file, char *str);

bool byteReadUint_8(FILE *file, uint8_t *ch);
bool byteReadUint_16(FILE *file, uint16_t *num);
bool byteReadUint_32(FILE *file, uint32_t *num);
bool byteReadUint_64(FILE *file, uint64_t *num);
bool byteReadStr(FILE *file, char **str);
#endif //AFUN__TOOL_H
