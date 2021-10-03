#include <stdio.h>
#include <stdlib.h>
#include "regex.h"

int main() {
    af_Regex *rg = makeRegex("Hello嘿.*d");
    int rc1 = matchRegex("Hello嘿World", rg);
    int rc2 = matchRegex("Nossss", rg);
    freeRegex(rg);
    if (rc1 == 1 && rc2 == 0) {
        printf("Success\n");
        return 0;
    }
    printf("Failed: %d, %d\n", rc1, rc2);
    return 1;
}