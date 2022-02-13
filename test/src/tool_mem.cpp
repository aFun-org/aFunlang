#include "aFuntool.h"

int main() {
    int *p = aFuntool::safeCalloc<int>();
    *p = 10;
    aFuntool::safeFree(p);

    p = aFuntool::safeCalloc<int>(1);
    *p = 10;
    aFuntool::safeFree(p);
    return 0;
}
