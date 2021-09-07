#ifndef AFUN_PARSER_H
#define AFUN_PARSER_H
#include "macro.h"
#include "token.h"
#include "reader.h"

#define SYNTACTIC_MAX_DEEP (1000)
typedef struct af_Parser af_Parser;

/* Parser 创建与释放 */
af_Parser *makeParser(DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func, size_t data_size,
                      FILE *error);
void freeParser(af_Parser *parser);
af_Parser *makeParserByString(char *str, bool free_str, FILE *error);
af_Parser *makeParserByFile(FilePath path, FILE *error);

/* Parser 操作函数 */
af_Code *parserCode(af_Parser *parser);
af_TokenType getTokenFromLexical(char **text, af_Parser *parser);
void *getParserData(af_Parser *parser);
void initParser(af_Parser *parser);

#endif //AFUN_PARSER_H
