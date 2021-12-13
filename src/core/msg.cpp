#include "msg.hpp"
using namespace aFuncore;
using namespace aFuntool;

aFuncore::Message::Message(const std::string &type){
    this->type = type;
    next = nullptr;
}

MessageStream::MessageStream(){
    stream = nullptr;
}

MessageStream::~MessageStream(){
    for (Message *msg = stream; msg != nullptr; msg = msg->next)
        delete msg;
}

void MessageStream::pushMessage(Message *msg){
    msg->next = stream;
    stream = msg;
}

Message *MessageStream::_getMessage(const std::string &type) const{
    for (Message *msg = stream; msg != nullptr; msg = msg->next) {
        if (msg->type == type)
            return msg;
    }
    return nullptr;
}

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
