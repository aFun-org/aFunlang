#ifndef AFUN_MAIN_RUN_H_
#define AFUN_MAIN_RUN_H_
#include "main.hpp"

enum RunListType {
    rl_file,
    rl_file_s,
    rl_file_b,
    rl_string
};

typedef struct RunList RunList;
struct RunList {
    enum RunListType type;

    union {
        FilePath file;
        char *string;
    };

    bool import;
    struct RunList *next;
};

RunList *makeFileRunList(FilePath file, bool import);
RunList *makeFileByteRunList(FilePath file);
RunList *makeFileSourceRunList(FilePath file);
RunList *makeStringRunList(char *string, bool import);
void freeAllRunList(RunList *rl);
RunList **pushRunList(RunList *rl, RunList **base);

int runCodeFromRunList(RunList *run_list, RunList **bak, bool save_afb, af_Environment *env);

#endif //AFUN___MAIN_RUN_H
