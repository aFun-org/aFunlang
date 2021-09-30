#ifndef AFUN_MAIN_RUN_H_
#define AFUN_MAIN_RUN_H_
#include "main.h"

typedef struct RunList RunList;
struct RunList {
    enum RunListType {
        rl_file,
        rl_file_s,
        rl_file_b,
        rl_string
    } type;

    union {
        FilePath file;
        char *string;
    };

    bool import;
    struct RunList *next;
};

RunList *makeFileRunList(FilePath file, bool import);
RunList *makeFileByteRunList(FilePath file, bool import);
RunList *makeFileSourceRunList(FilePath file, bool import);
RunList *makeStringRunList(char *string, bool import);
void freeAllRunList(RunList *rl);
RunList **pushRunList(RunList *rl, RunList **base);

int runCodeFromRunList(RunList *run_list, RunList **bak, bool save_afb, af_Environment *env);

#endif //AFUN___MAIN_RUN_H
