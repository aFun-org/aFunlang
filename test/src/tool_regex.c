#include <stdio.h>
#include <stdlib.h>
#include "regex.h"

int main() {
    af_Regex *rg = makeRegex("Hello嘿.*d", NULL);
    int rc1 = matchRegex("Hello嘿World", rg, NULL);
    int rc2 = matchRegex("Nossss", rg, NULL);
    freeRegex(rg);
    if (rc1 != 1 || rc2 != 0) {
        printf("Failed rg1: %d/1, %d/0\n", rc1, rc2);
        return 1;
    } else
        printf("rg1 success\n");

    af_Regex *rg2 = makeRegex("你|好", NULL);
    int rc3 = matchRegex("你", rg2, NULL);
    int rc4 = matchRegex("Nosssss", rg2, NULL);
    freeRegex(rg2);
    if (rc3 != 1 || rc4 != 0) {
        printf("Failed rg2: %d/1, %d/0\n", rc1, rc2);
        return 1;
    } else
        printf("rg2 success\n");

    printf("All success\n");
    return 0;
}