#include "msg.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto *um = new DownMessage();
    um->pushMessage(new Message("test-1"));
    std::cout << um->getMessage<Message>("test-1") << std::endl;
    delete um;
    return 0;
}
