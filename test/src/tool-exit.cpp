#include <iostream>
#include "aFuntool.h"
using namespace aFuntool;

void exit_func_push1(void *) {
    std::cout << "I am exit push-1" << std::endl;
}

void exit_func_push2(void *) {
    std::cout << "I am exit push-2" << std::endl;
}

int main(int argc, char **argv) {
    aFunAtExit(exit_func_push1, nullptr);
    aFunAtExit(exit_func_push2, nullptr);
    aFunExit(0);
}
