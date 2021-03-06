#include "core_env_var.h"

int main() {
    auto *evs = new aFuncore::EnvVarSpace();

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