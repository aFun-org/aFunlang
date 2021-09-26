#include "aFun.h"

int buildFileOutput(FilePath out, FilePath in, bool force) {
    if (!force) {
        time_t time_1 = getFileMTime(in);
        time_t time_2 = getFileMTime(out);

        if (time_1 == 0 && time_2 == 0) {
            fprintf(stderr, "File not exists [%s].", in);
            return -1;
        }

        if (time_2 >= time_1) {
            fprintf(stdout, "[aFunlang] Source already build (%s), use --force to build again.\n", in);
            return 0;
        }
    }

    fprintf(stdout, "[aFunlang] File (%s) will be build. (%s)\n", in, out);
    return buildFile(out, in, stderr);
}

int buildFileToPath(FilePath path, FilePath in, bool force) {
    char *name = getFileName(in);
    char *out = joinPath(path, name, ".aub");

    int res = buildFileOutput(out, in, force);

    free(name);
    free(out);
    return res;
}

int buildFileToSelf(FilePath in, bool force) {
    char *path = getFileNameWithPath(in);
    char *out = strJoin(path, ".aub", true, false);

    int res = buildFileOutput(out, in, force);

    free(out);
    return res;
}
