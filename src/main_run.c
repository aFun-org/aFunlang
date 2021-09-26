﻿#include "aFun.h"
#include "main_run.h"


static RunList *makeRunList() {
    RunList *run_list = calloc(1, sizeof(RunList));
    return run_list;
}

static RunList *freeRunList(RunList *rl) {
    RunList *next = rl->next;
    if (rl->type == rl_file_s || rl->type == rl_file_b || rl->type == rl_file)
        free(rl->file);
    else
        free(rl->string);
    free(rl);
    return next;
}

void freeAllRunList(RunList *rl) {
    while(rl != NULL)
        rl = freeRunList(rl);
}

RunList *makeFileRunList(FilePath file) {
    RunList *rl = makeRunList();
    rl->type = rl_file;
    rl->file = strCopy(file);
    return rl;
}

RunList *makeFileSourceRunList(FilePath file) {
    RunList *rl = makeFileRunList(file);
    rl->type = rl_file_s;
    return rl;
}

RunList *makeFileByteRunList(FilePath file) {
    RunList *rl = makeFileRunList(file);
    rl->type = rl_file_b;
    return rl;
}

RunList *makeStringRunList(char *string) {
    RunList *rl = makeRunList();
    rl->type = rl_string;
    rl->string = strCopy(string);
    return rl;
}

RunList **pushRunList(RunList *rl, RunList **base) {
    while (*base != NULL)
        base = &((*base)->next);
    *base = rl;
    while (*base != NULL)
        base = &((*base)->next);
    return base;
}

int runCodeFromRunList(RunList *run_list, RunList **bak, bool save_afb, af_Environment *env) {
    int exit_code = 0;

    for (NULL; run_list != NULL; run_list = run_list->next) {
        switch (run_list->type) {
            case rl_string:
                exit_code = runCodeFromString(run_list->string, "command-line-eval", stderr, env);
                break;
            case rl_file:
                exit_code = runCodeFromFile(run_list->file, stderr, save_afb, env);
                break;
            case rl_file_b:
                exit_code = runCodeFromFileByte(run_list->file, stderr, env);
                break;
            case rl_file_s:
                exit_code = runCodeFromFileSource(run_list->file, stderr, save_afb, NULL, env);
                break;
            default:
                break;
        }

        if (isCoreExit(env) == 1) {
            fprintf((exit_code == 0) ? stdout : stderr, "[aFunlang] aFun core exit, exit_code = %d\n", exit_code);
            break;
        }
    }

    if (bak != NULL)
        *bak = run_list;
    return exit_code;
}
