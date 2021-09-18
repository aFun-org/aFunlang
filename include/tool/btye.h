#ifndef AFUN_BYTE_H
#define AFUN_BYTE_H
#include <stdio.h>

// byte工具
#define byteWriteInt_8(file, s) (byteWriteUint_8(file, ((uint8_t)(s))))
#define byteWriteInt_16(file, s) (byteWriteUint_16(file, ((uint16_t)(s))))
#define byteWriteInt_32(file, s) (byteWriteUint_32(file, ((uint32_t)(s))))
#define byteWriteInt_64(file, s) (byteWriteUint_64(file, ((uint64_t)(s))))

#define byteReadInt_8(file, s) (byteReadUint_8(file, ((uint8_t *)(s))))
#define byteReadInt_16(file, s) (byteReadUint_16(file, ((uint16_t *)(s))))
#define byteReadInt_32(file, s) (byteReadUint_32(file, ((uint32_t *)(s))))
#define byteReadInt_64(file, s) (byteReadUint_64(file, ((uint64_t *)(s))))

enum af_EndianType{
    little_endian = 0,
    big_endian
};

extern enum af_EndianType endian;

void getEndian();
bool byteWriteUint_8(FILE *file, uint8_t ch);
bool byteWriteUint_16(FILE *file, uint16_t num);
bool byteWriteUint_32(FILE *file, uint32_t num);
bool byteWriteUint_64(FILE *file, uint64_t num);
bool byteWriteStr(FILE *file, char *str);

bool byteReadUint_8(FILE *file, uint8_t *ch);
bool byteReadUint_16(FILE *file, uint16_t *num);
bool byteReadUint_32(FILE *file, uint32_t *num);
bool byteReadUint_64(FILE *file, uint64_t *num);
bool byteReadStr(FILE *file, char **str);

#endif //AFUN_BYTE_H
