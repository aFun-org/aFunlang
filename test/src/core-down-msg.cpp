#include "msg.h"
int main() {
    auto *um = new aFuncore::DownMessage();
    um->pushMessage("test-1", new aFuncore::Message());
    std::cout << um->getMessage<aFuncore::Message>("test-1") << std::endl;
    delete um;
    return 0;
}
