#include <stdio.h>
#include "aFun.h"
#include "main_run.h"
#include "main_build.h"

ff_defArg(help, false)
                ff_argRule('h', help, not, 'h')
                ff_argRule('v', version, not, 'v')
ff_endArg(help, false);

ff_defArg(run, true)
                ff_argRule('e', eval, must, 'e')
                ff_argRule('f', file, must, 'f')
                ff_argRule('s', source, must, 's')
                ff_argRule('b', byte, must, 'b')
                ff_argRule(NUL, no-afb, not, 'a')
                ff_argRule(NUL, no-cl, not, 'n')
ff_endArg(run, true);

ff_defArg(build, false)
                ff_argRule('o', out, must, 'o')
                ff_argRule('p', path, must, 'p')
                ff_argRule('f', fource, not, 'f')
ff_endArg(build, false);

// exe 是指可执行程序, 而非仅PE的可执行程序
ff_childList(aFunlang_exe, ff_child(help), ff_child(run), ff_child(build));

static const char *name = NULL;

static int mainHelp(ff_FFlags *ff);
static void printVersion(void);
static void printHelp(void);
static int mainRun(ff_FFlags *ff);
static int mainBuild(ff_FFlags *ff);

int main(int argc, char **argv) {
    int exit_code = EXIT_SUCCESS;
    ff_FFlags *ff = ff_initFFlags(argc, argv, true, false, aFunlang_exe);
    if (ff == NULL)
        return EXIT_FAILURE;
    char *child = ff_getChild(ff);
    name = *argv;  // 获取第一个参数为name

    if (EQ_STR(child, "run"))
        exit_code = mainRun(ff);
    else if (EQ_STR(child, "build"))
        exit_code = mainBuild(ff);
    else
        exit_code = mainHelp(ff);

    ff_freeFFlags(ff);
    return exit_code;
}

static void printVersion(void) {
    printf("aFunlang at %s\n", name);
    printf("version: " aFunVersion "\n");
    printf(aFunDescription "\n");
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
    if (!have_opt) {
        printHelp();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

static RunList *getRunList(ff_FFlags *ff, bool *command_line, bool *save_afb) {
    char *text = NULL;
    RunList *run_list = NULL;
    RunList **prl = &run_list;
    int mark;

    *command_line = true;
    *save_afb = true;

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
            case 'a':
                *save_afb = false;
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
    bool save_afb = true;
    int exit_code;
    RunList *rl = getRunList(ff, &command_line, &save_afb);

    af_Environment *env = creatAFunEnviroment();
    if (rl != NULL)
        exit_code = runCodeFromRunList(rl, NULL, save_afb, env);
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

static int mainBuild(ff_FFlags *ff) {
    char *text = NULL;
    char *out_put = NULL;
    char *path = NULL;
    bool force = false;
    int mark;

    while (1) {
        mark = ff_getopt(&text, ff);

        switch (mark) {
            case 'o':
                if (path != NULL) {
                    fprintf(stderr, "Parameter conflict.\n");
                    goto error;
                }
                out_put = text;
                break;
            case 'p':
                if (out_put != NULL) {
                    fprintf(stderr, "Parameter conflict.\n");
                    goto error;
                }
                path = text;
                break;
            case 'f':
                force = true;
                break;
            case -1:
                goto out;
            default:
                printError(ff);
                return EXIT_FAILURE;
        }
    }

out:
    if (out_put != NULL) {
        FilePath in = NULL;

        /* 如果没有参数 */
        if (!ff_getopt_wild(&text, ff)) {
            fprintf(stderr, "No source file to build.\n");
            printHelp();
            return EXIT_FAILURE;
        } else
            in = text;

        /* 如果还有第二个参数 */
        if (ff_getopt_wild(&text, ff)) {
            fprintf(stderr, "Too many source file to build.\n");
            printHelp();
            return EXIT_FAILURE;
        }

        return buildFileOutput(out_put, in, force);
    } else if (path != NULL) {
        int exit_code = 0;
        while (ff_getopt_wild(&text, ff) && exit_code == 0)
            exit_code = buildFileToPath(path, text, force);
        return exit_code;
    }

    int exit_code = 0;
    while (ff_getopt_wild(&text, ff) && exit_code == 0)
        exit_code = buildFileToSelf(text, force);
    return exit_code;

error:
    printHelp();
    return EXIT_FAILURE;
}