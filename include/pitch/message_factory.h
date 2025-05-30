//
// Created by phamanhtan on 30/5/25.
//

#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include "seq_unit_header.h"
#include <memory>
#include "message.h"

namespace CboePitch {
    class MessageFactory {
    public:
        static std::unique_ptr<Message> parseMessage(const uint8_t *data, size_t size);

        static SeqUnitHeader parseHeader(const uint8_t *data, size_t size) {
            return SeqUnitHeader::parse(data, size);
        }
    };
} // namespace CboePitch

#endif // MESSAGE_FACTORY_H
