#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include "message.h"

namespace CboePitch {
    class AddOrder : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x37;
        static constexpr size_t MESSAGE_SIZE = 42;

        static AddOrder parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("AddOrder too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            char side = static_cast<char>(data[offset + 18]);
            uint32_t quantity = Message::readUint32LE(data + offset + 19);
            std::string symbol(reinterpret_cast<const char *>(data + offset + 23), 6);
            std::string participantId(reinterpret_cast<const char *>(data + offset + 37), 4);
            // Reserved byte at offset + 41 is ignored
            const double price = Message::decodePrice(data + offset + 29);

            return AddOrder(timestamp, orderId, side, quantity, symbol, price, participantId);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "AddOrder{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", side=" << sideIndicator
                    << ", quantity=" << quantity
                    << ", symbol=" << symbol
                    << ", price=" << price
                    << ", participantId=" << participantId
                    << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        // Getters
        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        char getSide() const { return sideIndicator; }
        uint32_t getQuantity() const { return quantity; }
        std::string getSymbol() const { return symbol; }
        uint64_t getPrice() const { return price; }
        std::string getParticipantId() const { return participantId; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        char sideIndicator;
        uint32_t quantity;
        std::string symbol;
        uint64_t price;
        std::string participantId;
        uint8_t data;

        AddOrder(uint64_t ts, uint64_t ordId, char side, uint32_t qty,
                 const std::string &sym, uint64_t prc, const std::string &pid, uint8_t data = 0)
            : timestamp(ts), orderId(ordId), sideIndicator(side),
              quantity(qty), symbol(sym), price(prc), participantId(pid) {
        }
    };
} // namespace CboePitch

#endif // ADD_ORDER_H
