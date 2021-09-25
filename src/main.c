#include <stdio.h>
#include "aFun.h"
#include "__main_run.h"

ff_defArg(help, true)
                ff_argRule('h', help, not, 'h')
                ff_argRule('v', version, not, 'v')
ff_endArg(help, true);

ff_defArg(run, false)
                ff_argRule('e', eval, must, 'e')
                ff_argRule('f', file, must, 'f')
                ff_argRule('s', source, must, 's')
                ff_argRule('b', byte, must, 'b')
                ff_argRule(NUL, no-cl, not, 'n')
ff_endArg(run, false);

ff_defArg(build, false)
                ff_argRule('o', out, not, 'o')
                ff_argRule('f', fource, not, 'f')
ff_endArg(build, false);

// exe 是指可执行程序, 而非仅PE的可执行程序
ff_childList(aFunlang_exe, ff_child(help), ff_child(run), ff_child(build));

static const char *name = NULL;

static int mainHelp(ff_FFlags *ff);
static void printVersion(void);
static void printHelp(void);
static int mainRun(ff_FFlags *ff);

int main(int argc, char **argv) {
    int exit_code = EXIT_SUCCESS;
    ff_FFlags *ff = ff_initFFlags(argc, argv, aFunlang_exe);
    if (ff == NULL)
        return EXIT_FAILURE;
    char *child = ff_getChild(ff);
    name = *argv;  // 获取第一个参数为name

    if (EQ_STR(child, "run"))
        exit_code = mainRun(ff);
    else if (EQ_STR(child, "build"))
        printf("build");
    else
        exit_code = mainHelp(ff);

    ff_freeFFlags(ff);

    printf("Enter any key to continue...");
    getc(stdin);
    return exit_code;
}

static void printVersion(void) {
    printf("aFunlang-varsion %s\n", name);
    printf("0.0.1\n");
}

static void printHelp(void) {
    printf("aFunlang Usage:\n");
    printf("Github: <https://github.com/aFun-org/aFunlang>\n");
}

/*
 * 函数名: printError
 * 目标: 打印参数错误信息
 */
static void printError(ff_FFlags *ff) {
    fprintf(stderr, "Command line argumenterror (%s)\n", ff_getChild(ff));
    printHelp();
}

static int mainHelp(ff_FFlags *ff) {
    char *text = NULL;
    int mark;
    bool have_opt = false;

    while (1) {
        mark = ff_getopt(&text, ff);

        switch (mark) {
            case 'h':
                printHelp();
                break;
            case 'v':
                printVersion();
                break;
            case -1:
                goto out;
            default:
                printError(ff);
                return EXIT_FAILURE;
        }

        have_opt = true;  // 表示有实际参数
    }
out:
    if (ff_getopt_wild(&text, ff))
        return EXIT_FAILURE;

    if (!have_opt) {
        printHelp();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

static RunList *getRunList(ff_FFlags *ff, bool *command_line) {
    char *text = NULL;
    RunList *run_list = NULL;
    RunList **prl = &run_list;
    int mark;

    while (1) {
        mark = ff_getopt(&text, ff);
        switch (mark) {
            case 'e':
                prl = pushRunList(makeStringRunList(text), prl);
                break;
            case 'f':
                prl = pushRunList(makeFileRunList(text), prl);
                break;
            case 's':
                prl = pushRunList(makeFileSourceRunList(text), prl);
                break;
            case 'b':
                prl = pushRunList(makeFileByteRunList(text), prl);
                break;
            case 'n':
                *command_line = false;
                break;
            case -1:
                goto out;
            default:
                printError(ff);
                freeAllRunList(run_list);
                return NULL;
        }
    }

out:
    while (ff_getopt_wild(&text, ff))
        prl = pushRunList(makeFileRunList(text), prl);
    return run_list;
}

static int mainRun(ff_FFlags *ff) {
    bool command_line = true;
    int exit_code;
    RunList *rl = getRunList(ff, &command_line);

    af_Environment *env = creatAFunEnviroment();
    if (rl != NULL)
        exit_code = runCodeFromRunList(rl, NULL, env);
    else if (!command_line) {
        fprintf(stderr, "Not code to run.\n");
        printHelp();
        return EXIT_FAILURE;
    }

    if (command_line) {
        while (isCoreExit(env) != 1)
            exit_code = runCodeFromStdin("stdin", stderr, env);
    }

    destructAFunEnvironment(env);
    freeAllRunList(rl);
    return exit_code;
}