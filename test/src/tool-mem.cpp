#include "aFuntool.h"

int main() {
    int *p = calloc(1, int);
    *p = 10;
    free(p);

    p = calloc(1, int);
    *p = 10;
    free(p);
    return 0;
}
