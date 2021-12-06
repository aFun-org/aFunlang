#include <cstdio>
#include <cstdlib>
#include "regex.hpp"

int main() {
    af_Regex *rg = makeRegex("Hello嘿.*d", nullptr);
    int rc1 = matchRegex("Hello嘿World", rg, nullptr);
    int rc2 = matchRegex("Nossss", rg, nullptr);
    freeRegex(rg);
    if (rc1 != 1 || rc2 != 0) {
        printf("Failed rg1: %d/1, %d/0\n", rc1, rc2);
        return 1;
    } else
        printf("rg1 success\n");

    af_Regex *rg2 = makeRegex("你|好", nullptr);
    int rc3 = matchRegex("你", rg2, nullptr);
    int rc4 = matchRegex("Nosssss", rg2, nullptr);
    freeRegex(rg2);
    if (rc3 != 1 || rc4 != 0) {
        printf("Failed rg2: %d/1, %d/0\n", rc1, rc2);
        return 1;
    } else
        printf("rg2 success\n");

    printf("All success\n");
    return 0;
}