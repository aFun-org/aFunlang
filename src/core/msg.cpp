﻿#include "msg.h"
#include "core-activation.h"
#include "inter.h"
#include "env-var.h"

namespace aFuncore {
    NormalMessage::NormalMessage(Object *obj_) : obj {obj_} {
        obj->addReference();
    }

    NormalMessage::~NormalMessage(){
        if (obj != nullptr) {
            obj->delReference();
            obj = nullptr;
        }
    }

    void NormalMessage::topProgress(Inter &inter, Activation &){
        inter.getOutMessageStream().pushMessage("NORMAL", new NormalMessage(std::move(*this)));
    }

    ErrorMessage::ErrorMessage(std::string error_type_, std::string error_info_, Activation *start)
        : inter{start->inter}, error_type{std::move(error_type_)}, error_info{std::move(error_info_)} {
        for (const auto activation : inter.getStack()) {
            if (activation->getFileLine() != 0)
                trackback.push_front({activation->getFilePath(), activation->getFileLine()});
        }
    }

    void ErrorMessage::topProgress(Inter &inter_, Activation &){
        inter_.getOutMessageStream().pushMessage("ERROR", new ErrorMessage(std::move(*this)));
    }

    MessageStream::~MessageStream(){
        for (auto &msg : stream)
            delete msg.second;
    }

    /**
     * 压入 Message
     * @param msg Message
     */
    void MessageStream::pushMessage(const std::string &type, Message *msg){
        stream.emplace(type, msg);
    }

    /**
     * 获取 Message
     * @param type 类型
     * @return Message
     */
    Message *MessageStream::_getMessage(const std::string &type) const{
        auto ret = stream.find(type);
        if (ret == stream.end())
            return nullptr;
        return ret->second;
    }

    /**
     * 弹出Message (使Message脱离数据流)
     * @param type 类型
     * @return Message
     */
    Message *MessageStream::popMessage(const std::string &type){
        auto ret = stream.find(type);
        if (ret == stream.end())
            return nullptr;
        Message *msg = ret->second;
        stream.erase(ret);
        return msg;
    }

    UpMessage::UpMessage(const UpMessage *old_) : MessageStream(), old{old_} {

    }

    Message *UpMessage::_getMessage(const std::string &type) const {
        for (const UpMessage *up = this; up != nullptr; up = up->old) {
            Message *ret = up->MessageStream::_getMessage(type);
            if (ret != nullptr)
                return ret;
        }
        return nullptr;
    }

    /**
     * 拼接数据流 (将this合并到msg)
     * @param msg
     */
    void DownMessage::joinMsg(DownMessage &msg){
        msg.stream.merge(stream);
    }

    Message *InterMessage::popFrontMessage(std::string &type) {
        std::unique_lock<std::mutex> mutex{lock};
        if (stream.empty())
            return nullptr;
        Message *ret = stream.begin()->second;
        type = stream.begin()->first;
        stream.erase(stream.begin());
        return ret;
    }

    Message *InterMessage::popMessage(const std::string &type) {
        std::unique_lock<std::mutex> mutex{lock};
        return MessageStream::popMessage(type);
    }

    void InterMessage::pushMessage(const std::string &type, Message *msg) {
        std::unique_lock<std::mutex> mutex{lock};
        MessageStream::pushMessage(type, msg);
    }
}