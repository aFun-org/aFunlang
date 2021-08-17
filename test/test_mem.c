#include "mem.h"

int main() {
    int *p = calloc(1, sizeof(int));
    *p = 10;
    free(p);

    p = calloc(1, sizeof(int));
    *p = 10;
    free(p);
    return 0;
}
