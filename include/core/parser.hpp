#ifndef AFUN_PARSER_HPP
#define AFUN_PARSER_HPP
#include "aFunCoreExport.h"
#include "tool.hpp"
#include "token.h"
#include "reader.hpp"

#define SYNTACTIC_MAX_DEEP (1000)
typedef struct af_Parser af_Parser;

#define STDIN_MAX_SIZE (1024)

/* Parser 创建与释放 */
AFUN_CORE_EXPORT af_Parser *makeParser(FilePath file,
                                       DLC_SYMBOL(readerFunc) read_func, DLC_SYMBOL(destructReaderFunc) destruct_func,
                                       size_t data_size);
AFUN_CORE_EXPORT void freeParser(af_Parser *parser);
AFUN_CORE_EXPORT af_Parser *makeParserByString(ConstFilePath name, const char *str, bool free_str);
AFUN_CORE_EXPORT af_Parser *makeParserByFile(ConstFilePath path);
AFUN_CORE_EXPORT af_Parser *makeParserByStdin(ConstFilePath file);

/* Parser 相关操作 */
AFUN_CORE_EXPORT af_Code *parserCode(af_Parser *parser);
AFUN_CORE_EXPORT void initParser(af_Parser *parser);

#endif //AFUN_PARSER_HPP
