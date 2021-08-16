#include "mem.h"

int main() {
    int *p = safeCalloc(1, sizeof(int));
    *p = 10;
    safeFree(p);

    p = safeCalloc(1, sizeof(int));
    *p = 10;
    safeFree_(p);
    return 0;
}
