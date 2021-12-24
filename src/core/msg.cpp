#include "msg.hpp"
using namespace aFuncore;
using namespace aFuntool;

NormalMessage::NormalMessage(Object *obj) : TopMessage("NORMAL") {
    this->obj = obj;
}

NormalMessage::~NormalMessage(){
    this->obj = nullptr;
}

void NormalMessage::topProgress(){
    printf_stdout(0, "NORMAL: %p\n", obj);
}

MessageStream::MessageStream(){
    stream = nullptr;
}

MessageStream::~MessageStream(){
    for (Message *msg = stream, *tmp; msg != nullptr; msg = tmp) {
        tmp = msg->next;
        delete msg;
    }
}

/**
 * 压入 Message
 * @param msg Message
 */
void MessageStream::pushMessage(Message *msg){
    msg->next = stream;
    stream = msg;
}

/**
 * 获取 Message
 * @param type 类型
 * @return Message
 */
Message *MessageStream::_getMessage(const std::string &type) const{
    for (Message *msg = stream; msg != nullptr; msg = msg->next) {
        if (msg->type == type)
            return msg;
    }
    return nullptr;
}

/**
 * 弹出Message (使Message脱离数据流)
 * @param type 类型
 * @return Message
 */
Message *MessageStream::popMessage(const std::string &type) {
    for (Message **msg = &stream; *msg != nullptr; msg = &((*msg)->next)) {
        if ((*msg)->type == type) {
            Message *ret = *msg;
            *msg = ret->next;
            return ret;
        }
    }
    return nullptr;
}

UpMessage::UpMessage(const UpMessage *old) : MessageStream() {
    if (old != nullptr)
        this->old = old->stream;
    else
        this->old = nullptr;
    this->stream = this->old;
}

UpMessage::~UpMessage(){
    if (old != nullptr) {
        for (Message **msg = &stream; *msg != nullptr; msg = &((*msg)->next)) {
            if (*msg == old) {
                *msg = nullptr;
                break;
            }
        }
    }
}

/**
 * 弹出Message (使Message脱离数据流)
 * 注意: 不会弹出继承的Message
 * @param type 类型
 * @return Message
 */
Message *UpMessage::popMessage(const std::string &type){
    for (Message **msg = &stream; *msg != nullptr; msg = &((*msg)->next)) {
        if ((*msg) == old)
            break;
        if ((*msg)->type == type) {
            Message *ret = *msg;
            *msg = ret->next;
            return ret;
        }
    }
    return nullptr;
}

/**
 * 拼接数据流
 * @param msg
 */
void DownMessage::joinMsg(DownMessage *msg){
    Message *m = stream;
    if (m == nullptr)
        return;
    while (m->next != nullptr)
        m = m->next;
    m->next = msg->stream;
    msg->stream = m;
    stream = nullptr;
}
