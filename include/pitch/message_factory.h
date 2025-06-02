#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include <memory>
#include <vector>
#include "message.h"

namespace CboePitch {
    class SeqUnitHeader;
    class Message;

    class MessageFactory {
    public:
        static std::shared_ptr<Message> parseMessage(const uint8_t* data, size_t size);
        static SeqUnitHeader parseHeader(const uint8_t* data, size_t size);
        static std::vector<std::shared_ptr<Message>> parseMessages(const uint8_t* data, size_t size);
    };
} // namespace CboePitch

#endif // MESSAGE_FACTORY_H