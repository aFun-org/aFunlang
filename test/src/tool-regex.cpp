#include <cstdio>
#include <iostream>
#include "aFuntool.h"

int main() {
    Regex *rg = new Regex("Hello嘿.*d");
    int rc1 = rg->match("Hello嘿World");
    int rc2 = rg->match("Nossss");
    delete rg;

    if (rc1 != 1 || rc2 != 0) {
        printf("Failed rg1: %d/1, %d/0\n", rc1, rc2);
        return 1;
    } else
        printf("rg1 success\n");

    Regex *rg2 = new Regex("你|好");
    int rc3 = rg2->match("你");
    int rc4 = rg2->match("Nosssss");
    delete rg2;

    if (rc3 != 1 || rc4 != 0) {
        printf("Failed rg2: %d/1, %d/0\n", rc1, rc2);
        return 1;
    } else
        printf("rg2 success\n");

    printf("All success\n");
    return 0;
}