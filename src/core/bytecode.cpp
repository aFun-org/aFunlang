#include "aFunCore.hpp"
#include "tool.hpp"

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
        nullptr,  // 无错误
        "File open error.",  // 2
        "Code md5 error.",  // 3
        "Write code error."  // 4
};

char const * const readByteCodeError[7] = {
        "File read error.",  // 0
        nullptr,  // 无错误
        "File open error.",  // 2
        "The file is not an aFun Bytecode file.",  // 3
        "Incompatible version.",  // 4
        "Read code error.",  // 5
        "Code is broken."  // 6
};

int writeByteCode(af_Code *code, ConstFilePath path) {
    if (code == nullptr || path == nullptr)
        return 0;

    char *md5 = getCodeMD5(code);
    if (md5 == nullptr)
        return 3;

    int re = 1;
    FILE *file = fileOpen(path, "wb");
    if (file == nullptr) {
        re = 2;
        goto RETURN_2;
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
    fileClose(file);
RETURN_2:
    free(md5);
    return re;
}

int readByteCode(af_Code **code, ConstFilePath path) {
    if (code == nullptr || path == nullptr || *code != nullptr)
        return 0;

    int re = 1;
    FILE *file = fileOpen(path, "rb");
    if (file == nullptr)
        return 2;

    char *head = nullptr;
    uint32_t major = 0;
    uint32_t minor = 0;
    uint32_t patch = 0;
    char *md5str = nullptr;
    char *code_md5 = nullptr;

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
        goto RETURN_;
    }

    code_md5 = getCodeMD5(*code);
    if (code_md5 == nullptr || !EQ_STR(code_md5, md5str)) {
        re = 6;
        freeAllCode(*code);
        goto RETURN_;
    }

RETURN_:
    free(head);
    free(md5str);
    free(code_md5);
    fileClose(file);
    return re;
}
