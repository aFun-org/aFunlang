#include "aFuntool.h"

int main() {
    int *p = aFuntool::safeCalloc<int>();
    *p = 10;
    free(p);

    p = aFuntool::safeCalloc<int>(1);
    *p = 10;
    free(p);
    return 0;
}
