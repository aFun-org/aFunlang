#include "core_message_stream.h"
#include "core_activation.h"
#include "core_inter.h"
#include "core_env_var.h"

namespace aFuncore {
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

    UpMessageStream::UpMessageStream(const UpMessageStream *old_) : MessageStream(), old{old_} {

    }

    Message *UpMessageStream::_getMessage(const std::string &type) const {
        for (const UpMessageStream *up = this; up != nullptr; up = up->old) {
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
    void DownMessageStream::joinMsg(DownMessageStream &msg){
        msg.stream.merge(stream);
    }

    Message *InterMessageStream::popFrontMessage(std::string &type) {
        std::unique_lock<std::mutex> mutex{lock};
        if (stream.empty())
            return nullptr;
        Message *ret = stream.begin()->second;
        type = stream.begin()->first;
        stream.erase(stream.begin());
        return ret;
    }

    Message *InterMessageStream::popMessage(const std::string &type) {
        std::unique_lock<std::mutex> mutex{lock};
        return MessageStream::popMessage(type);
    }

    void InterMessageStream::pushMessage(const std::string &type, Message *msg) {
        std::unique_lock<std::mutex> mutex{lock};
        MessageStream::pushMessage(type, msg);
    }
}