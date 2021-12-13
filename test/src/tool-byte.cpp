#include <cstdio>
#include "tool.hpp"
using namespace aFuntool;


#define TEST_WRITE(test, type) do{\
if (!(test)) { \
    fprintf(stderr, "Test write wrong: " #type "\n"); \
    return EXIT_FAILURE; \
}}while(0)

#define TEST_READ(test, type) do{\
if (!(test)) { \
fprintf(stderr, "Test read wrong: " #type "\n"); \
return EXIT_FAILURE; \
}}while(0)

int main() {
    int8_t test8 = 10;
    int16_t test16 = 20;
    int32_t test32 = 30;
    int64_t test64 = 40;
    const std::string testStr = "test";

    getEndian();

    FILE *file = fileOpen("test.byte", "wb");
    if (file == nullptr) {
        fprintf(stderr, "Can't not creat file: test.byte\n");
        return EXIT_FAILURE;
    }

    TEST_WRITE(byteWriteInt(file, test8), uint8_t);
    TEST_WRITE(byteWriteInt(file, test16), uint16_t);
    TEST_WRITE(byteWriteInt(file, test32), uint32_t);
    TEST_WRITE(byteWriteInt(file, test64), uint64_t);
    TEST_WRITE(byteWriteStr(file, testStr), str);

    fileClose(file);

    int8_t rtest8;
    int16_t rtest16;
    int32_t rtest32;
    int64_t rtest64;
    std::string rtestStr;

    file = fileOpen("test.byte", "rb");
    if (file == nullptr) {
        fprintf(stderr, "Can't not read file: test.byte\n");
        return EXIT_FAILURE;
    }

    TEST_READ(byteReadInt(file, (uint8_t *)&rtest8), uint8_t);
    TEST_READ(byteReadInt(file, (uint16_t *)&rtest16), uint16_t);
    TEST_READ(byteReadInt(file, (uint32_t *)&rtest32), uint32_t);
    TEST_READ(byteReadInt(file, (uint64_t *)&rtest64), uint64_t);
    TEST_READ(byteReadStr(file, rtestStr), str);

    if (rtest8 != test8 || rtest16 != test16 || rtest32 != test32 || rtest64 != test64 || rtestStr != testStr) {
        printf("error.\n");
        printf("rtest8 = %d, test = %d\n", rtest8, test8);
        printf("rtest16 = %d, test = %d\n", rtest16, test16);
        printf("rtest32 = %d, test = %d\n", rtest32, test32);
        printf("rtest64 = %ld, test = %ld\n", rtest64, test64);
        printf("rtestStr = %s\ntestStr = %s\n", rtestStr.c_str(), testStr.c_str());
        return EXIT_FAILURE;
    }

    fileClose(file);
    printf("success.\n");
    return EXIT_SUCCESS;
}