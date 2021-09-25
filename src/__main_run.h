#ifndef AFUN_MAIN_RUN_H_
#define AFUN_MAIN_RUN_H_

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

    struct RunList *next;
};

RunList *makeFileRunList(FilePath file);
RunList *makeFileByteRunList(FilePath file);
RunList *makeFileSourceRunList(FilePath file);
RunList *makeStringRunList(char *string);
void freeAllRunList(RunList *rl);
RunList **pushRunList(RunList *rl, RunList **base);

int runCodeFromRunList(RunList *run_list, RunList **bak, af_Environment *env);

#endif //AFUN___MAIN_RUN_H
