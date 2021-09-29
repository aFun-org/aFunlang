#include "aFunCore.h"
#include "tool.h"

#define HEAD "aFun-BytecodeFile"
#define Done(write) do{if(!(write)){re = 0; goto RETURN_;}}while(0)

/* aFun在一般情况下, 主版本号向下兼容 */
static bool checkVersion(uint32_t major, uint32_t minor, uint32_t patch) {
    if (major <= aFunMajorVersion)
        return true;
    return false;
}

char const * const writeByteCodeError[5] = {
        "File write error.",  // 0
        NULL,  // 无错误
        "File open error.",  // 2
        "Code md5 error.",  // 3
        "Write code error."  // 4
};

char const * const readByteCodeError[7] = {
        "File write error.",  // 0
        NULL,  // 无错误
        "File open error.",  // 2
        "The file is not an aFun Bytecode file.",  // 3
        "Incompatible version.",  // 4
        "Read code error.",  // 5
        "Code is broken."  // 6
};

int writeByteCode(af_Code *code, FilePath path) {
    if (code == NULL || path == NULL)
        return 0;

    char *md5 = getCodeMD5(code);
    if (md5 == NULL)
        return 3;

    int re = 1;
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        re = 2;
        goto RETURN_;
    }

    Done(byteWriteStr(file, HEAD));
    Done(byteWriteUint_32(file, aFunMajorVersion));
    Done(byteWriteUint_32(file, aFunMinorVersion));
    Done(byteWriteUint_32(file, aFunPatchVersion));
    Done(byteWriteStr(file, md5));
    if (!writeAllCode(code, file)) {
        re = 4;
        goto RETURN_;
    }


RETURN_:
    free(md5);
    return re;
}

int readByteCode(af_Code **code, FilePath path) {
    if (code == NULL || path == NULL || *code != NULL)
        return 0;

    int re = 1;
    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return 2;

    char *head = NULL;
    uint32_t major = 0;
    uint32_t minor = 0;
    uint32_t patch = 0;
    char *md5str = NULL;
    char *code_md5 = NULL;

    Done(byteReadStr(file, &head));
    if (!EQ_STR(head, HEAD)) {
        re = 3;
        goto RETURN_;
    }

    Done(byteReadUint_32(file, &major));
    Done(byteReadUint_32(file, &minor));
    Done(byteReadUint_32(file, &patch));
    Done(byteReadStr(file, &md5str));

    if (!checkVersion(major, minor, patch)) {
        re = 4;
        goto RETURN_;
    }

    if (!readAllCode(code, path, file)) {
        re = 5;
        freeAllCode(*code);
        goto RETURN_;
    }

    code_md5 = getCodeMD5(*code);
    if (code_md5 == NULL || !EQ_STR(code_md5, md5str)) {
        re = 6;
        freeAllCode(*code);
        goto RETURN_;
    }

RETURN_:
    free(head);
    free(md5str);
    free(code_md5);
    fclose(file);
    return re;
}
