#ifndef DELETE_ORDER_H
#define DELETE_ORDER_H

#include "message.h"
#include "SymbolIdentifier.hpp"
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class DeleteOrder : public Message {
    public:
        static constexpr size_t MESSAGE_SIZE = 18;
        static constexpr uint8_t MESSAGE_TYPE = 0x3C;

        static DeleteOrder parse(const uint8_t *data, size_t size, equix_md::SymbolIdentifier& symbol_map, size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("DeleteOrder message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);

            DeleteOrder delete_order(timestamp, orderId);
            delete_order.setSymbolMap(&symbol_map);
            delete_order.setPayload(data + offset, MESSAGE_SIZE);

            // Remove mapping from SymbolIdentifier
            if (!symbol_map.remove_mapping(orderId)) {
                std::cerr << "Warning: Order ID " << orderId << " not found in SymbolIdentifier" << std::endl;
            }

            return delete_order;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "DeleteOrder{timestamp=" << timestamp
                << ", orderId=" << orderId
                << ", symbol=" << getSymbol() << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }
        uint64_t getOrderId() const override { return orderId; }

        uint64_t getTimestamp() const { return timestamp; }

    private:
        uint64_t timestamp;
        uint64_t orderId;

        DeleteOrder(uint64_t ts, uint64_t ordId)
            : timestamp(ts), orderId(ordId) {}
    };
} // namespace CboePitch

#endif // DELETE_ORDER_H