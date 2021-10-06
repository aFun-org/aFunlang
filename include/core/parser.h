#ifndef AFUN_PARSER_H
#define AFUN_PARSER_H
#include "aFunCoreExport.h"
#include "tool.h"
#include "token.h"
#include "reader.h"

#define SYNTACTIC_MAX_DEEP (1000)
typedef struct af_Parser af_Parser;

typedef bool ParserStdinInterruptFunc(void);

#define STDIN_MAX_SIZE (1024)

/* Parser 创建与释放 */
AFUN_CORE_EXPORT af_Parser *
makeParser(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size);
AFUN_CORE_EXPORT void freeParser(af_Parser *parser);
AFUN_CORE_EXPORT af_Parser *makeParserByString(char *str, bool free_str);
AFUN_CORE_EXPORT af_Parser *makeParserByFile(FilePath path);
AFUN_CORE_EXPORT af_Parser *makeParserByStdin(ParserStdinInterruptFunc *interrupt);

/* Parser 相关操作 */
AFUN_CORE_EXPORT af_Code *parserCode(FilePath file, af_Parser *parser);
AFUN_CORE_EXPORT void initParser(af_Parser *parser);

#endif //AFUN_PARSER_H
