#include "rt_error_message.h"

namespace aFunrt {
    ErrorMessage::ErrorMessage(std::string error_type_, std::string error_info_, aFuncore::Inter &inter)
            : inter{inter}, error_type{std::move(error_type_)}, error_info{std::move(error_info_)} {
        for (const auto activation : inter.getStack()) {
            if (activation->getFileLine() != 0)
                trackback.push_front({activation->getFilePath(), activation->getFileLine()});
        }
    }

    void ErrorMessage::topProgress(aFuncore::Inter &inter_, aFuncore::Activation &){
        inter_.getOutMessageStream().pushMessage("ERROR", new ErrorMessage(std::move(*this)));
    }
}
