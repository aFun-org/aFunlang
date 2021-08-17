#include <stdio.h>

#if _MSC_VER
_declspec(dllimport) extern int num;
#else
extern int num;
#endif

int test(int a);

int main() {
    printf("num = %d test = %d\n", num, test(100));
    return 0;
}