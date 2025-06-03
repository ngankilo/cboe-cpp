#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include "message.h"
#include "seq_unit_header.h"
#include "SymbolIdentifier.hpp"
#include <memory>
#include <vector>
#include <cstdint>

namespace CboePitch {
    class MessageFactory {
    public:
        static std::shared_ptr<Message> parseMessage(const uint8_t *data, size_t size, equix_md::SymbolIdentifier& symbol_map);
        static SeqUnitHeader parseHeader(const uint8_t *data, size_t size);
        static std::vector<std::shared_ptr<Message>> parseMessages(const uint8_t *data, size_t length, equix_md::SymbolIdentifier& symbol_map);
        // static std::vector<std::shared_ptr<Message>> parseMessages(const uint8_t *data, size_t length);
    };
} // namespace CboePitch

#endif // MESSAGE_FACTORY_H