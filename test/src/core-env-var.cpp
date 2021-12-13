#include "env-var.hpp"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto *evs = new EnvVarSpace();

    evs->setString("a", "string");
    evs->setNumber("a", 20);

    std::string s;
    int32_t n;

    evs->findString("a", s);
    evs->findNumber("a", n);

    std::cout << "a = " << s << ", " << n << std::endl;

    delete evs;
    return 0;
}