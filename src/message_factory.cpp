#include "pitch/message_factory.h"
#include "pitch/message_dispatcher.h"
#include "pitch/seq_unit_header.h"

namespace CboePitch {
    std::shared_ptr<Message> MessageFactory::parseMessage(const uint8_t* data, size_t size) {
        if (size < 1) {
            throw std::invalid_argument("Message too short");
        }
        uint8_t messageType = data[0];
        const auto& dispatchTable = MessageDispatch::getDispatchTable();
        auto it = dispatchTable.find(messageType);
        if (it == dispatchTable.end()) {
            throw std::invalid_argument("Unknown message type: " + std::to_string(messageType));
        }

        return std::static_pointer_cast<Message>(it->second.parser(data, size)); // Cast unique_ptr to shared_ptr
    }

    SeqUnitHeader MessageFactory::parseHeader(const uint8_t* data, size_t size) {
        return SeqUnitHeader::parse(data, size);
    }
}