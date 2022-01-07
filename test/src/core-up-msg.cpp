#include "msg.h"
using namespace aFuncore;
using namespace aFuntool;

int main() {
    auto *um = new UpMessage(nullptr);
    um->pushMessage(new Message("test-1"));
    std::cout << um->getMessage<Message>("test-1") << std::endl;

    auto *um2 = new UpMessage(um);
    um2->pushMessage(new Message("test-2"));
    std::cout << um2->getMessage<Message>("test-1") << std::endl;
    std::cout << um2->getMessage<Message>("test-2") << std::endl;

    auto msg1 = um2->popMessage("test-2");
    auto msg2 = um2->popMessage("test-1");

    std::cout << msg1 << ", " << msg2 << std::endl;

    delete msg1;
    delete msg2;
    delete um2;

    std::cout << um->getMessage<Message>("test-1") << std::endl;
    std::cout << um->getMessage<Message>("test-2") << std::endl;

    delete um;
    return 0;
}