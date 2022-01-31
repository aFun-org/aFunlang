#include "msg.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto *um = new DownMessage();
    um->pushMessage("test-1", new Message());
    std::cout << um->getMessage<Message>("test-1") << std::endl;
    delete um;
    return 0;
}
