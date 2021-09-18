#include <stdio.h>
#include "aFun.h"
#include "tool.h"


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
    char *testStr = "test";

    getEndian();

    FILE *file = fopen("test.byte", "wb");
    if (file == NULL) {
        fprintf(stderr, "Can't not creat file: test.byte\n");
        return EXIT_FAILURE;
    }

    TEST_WRITE(byteWriteInt_8(file, test8), uint8_t);
    TEST_WRITE(byteWriteInt_16(file, test16), uint16_t);
    TEST_WRITE(byteWriteInt_32(file, test32), uint32_t);
    TEST_WRITE(byteWriteInt_64(file, test64), uint64_t);
    TEST_WRITE(byteWriteStr(file, testStr), str);

    fclose(file);

    int8_t rtest8;
    int16_t rtest16;
    int32_t rtest32;
    int64_t rtest64;
    char *rtestStr;

    file = fopen("test.byte", "rb");
    if (file == NULL) {
        fprintf(stderr, "Can't not read file: test.byte\n");
        return EXIT_FAILURE;
    }

    TEST_READ(byteReadInt_8(file, &rtest8), uint8_t);
    TEST_READ(byteReadInt_16(file, &rtest16), uint16_t);
    TEST_READ(byteReadInt_32(file, &rtest32), uint32_t);
    TEST_READ(byteReadInt_64(file, &rtest64), uint64_t);
    TEST_READ(byteReadStr(file, &rtestStr), str);

    if (rtest8 != test8 || rtest16 != test16 || rtest32 != test32 || rtest64 != test64 || !EQ_STR(rtestStr, testStr)) {
        printf("error.\n");
        printf("rtest8 = %d , test = %d\n", rtest8, test8);
        printf("rtest16 = %d, test = %d\n", rtest16, test16);
        printf("rtest32 = %d, test = %d\n", rtest32, test32);
        printf("rtest64 = %ld, test = %ld\n", rtest64, test64);
        printf("rtestStr = %s\ntestStr = %s\n", rtestStr, testStr);
        return EXIT_FAILURE;
    }

    free(rtestStr);
    printf("success.\n");
    return EXIT_SUCCESS;
}