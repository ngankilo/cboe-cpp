#ifndef DELETE_ORDER_H
#define DELETE_ORDER_H

#include "message.h"
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class DeleteOrder : public Message {
    public:
        static constexpr size_t MESSAGE_SIZE = 18;
        static constexpr uint8_t MESSAGE_TYPE = 0x3C;

        static DeleteOrder parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("DeleteOrder message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);

            return DeleteOrder(timestamp, orderId);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "DeleteOrder{timestamp=" << timestamp
                    << ", orderId=" << orderId << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }

        const std::string &getSymbol() const override {
            //TODO Implement logic get Symbol from map via order id
            static const std::string naSymbol = "N/A";
            return naSymbol;
        }

    private:
        uint64_t timestamp;
        uint64_t orderId;

        DeleteOrder(uint64_t ts, uint64_t ordId)
            : timestamp(ts), orderId(ordId) {
        }
    };
} // namespace CboePitch

#endif // DELETE_ORDER_H
