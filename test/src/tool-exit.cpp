#include <iostream>
#include "aFuntool.h"
using namespace aFuntool;

void exit_func(void *) {
    std::cout << "I am exit" << std::endl;
}

int main(int argc, char **argv) {
    aFunAtExit(exit_func, nullptr);
    aFunExit(0);
}
