#include "core-message-stream.h"

int main() {
    auto *um = new aFuncore::UpMessageStream(nullptr);
    um->pushMessage("test-1", new aFuncore::Message());
    std::cout << um->getMessage<aFuncore::Message>("test-1") << std::endl;

    auto *um2 = new aFuncore::UpMessageStream(um);
    um2->pushMessage("test-2", new aFuncore::Message());
    std::cout << um2->getMessage<aFuncore::Message>("test-1") << std::endl;
    std::cout << um2->getMessage<aFuncore::Message>("test-2") << std::endl;

    auto msg1 = um2->popMessage("test-2");
    auto msg2 = um2->popMessage("test-1");

    std::cout << msg1 << ", " << msg2 << std::endl;

    delete msg1;
    delete msg2;
    delete um2;

    std::cout << um->getMessage<aFuncore::Message>("test-1") << std::endl;
    std::cout << um->getMessage<aFuncore::Message>("test-2") << std::endl;

    delete um;
    return 0;
}