#include <stdio.h>

extern int num;
int test(int a);

int main() {
    printf("num = %d test = %d\n", num, test(num));
    return 0;
}