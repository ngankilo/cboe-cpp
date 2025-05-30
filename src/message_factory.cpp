//
// Created by phamanhtan on 30/5/25.
//

#include "pitch/message_factory.h"
#include "pitch/message_dispatcher.h"

namespace CboePitch {
    std::unique_ptr<Message> MessageFactory::parseMessage(const uint8_t *data, size_t size) {
        if (size < 1) {
            throw std::invalid_argument("Message too short");
        }

        uint8_t messageType = data[0];
        const auto &dispatchTable = MessageDispatch::getDispatchTable();
        auto it = dispatchTable.find(messageType);
        if (it == dispatchTable.end()) {
            throw std::invalid_argument("Unknown message type: " + std::to_string(messageType));
        }

        return it->second.parser(data, size);
    }
} // namespace CboePitch
