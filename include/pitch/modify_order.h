#ifndef MODIFY_ORDER_H
#define MODIFY_ORDER_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class ModifyOrder : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x3A;
        static constexpr size_t MESSAGE_SIZE = 31;

        static ModifyOrder parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < 31) {
                throw std::invalid_argument("ModifyOrder message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t quantity = Message::readUint32LE(data + offset + 18);
            double price = Message::decodePrice(data + offset + 22);

            // Price is fixed-point (nanodollars or 4 decimal places)
            // double price = static_cast<double>(rawPrice) / 10000000.0;

            return ModifyOrder(timestamp, orderId, quantity, price);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "ModifyOrder{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", quantity=" << quantity
                    << ", price=" << price << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        // Accessors
        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getQuantity() const { return quantity; }
        double getPrice() const { return price; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t quantity;
        double price;

        ModifyOrder(uint64_t ts, uint64_t ordId, uint32_t qty, double prc)
            : timestamp(ts), orderId(ordId), quantity(qty), price(prc) {
        }
    };
} // namespace CboePitch

#endif // MODIFY_ORDER_H
