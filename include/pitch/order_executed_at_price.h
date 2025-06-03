#ifndef ORDER_EXECUTED_AT_PRICE_H
#define ORDER_EXECUTED_AT_PRICE_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class OrderExecutedAtPrice : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x58;
        static constexpr size_t MESSAGE_SIZE = 52;

        static OrderExecutedAtPrice parse(const uint8_t *data, size_t size, equix_md::SymbolIdentifier &symbol_map,
                                          size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("OrderExecutedAtPrice message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t executedQuantity = Message::readUint32LE(data + offset + 18);
            uint64_t executionId = Message::readUint64LE(data + offset + 22);
            uint64_t contraOrderId = Message::readUint64LE(data + offset + 30);

            // Contra PID: 4 ASCII characters, right-padded with spaces
            std::string contraPid(reinterpret_cast<const char *>(data + offset + 38), 4);
            contraPid.erase(contraPid.find_last_not_of(' ') + 1); // trim trailing spaces

            char executionType = static_cast<char>(data[offset + 42]);

            double price = Message::decodePrice(data + offset + 43);

            // Reserved byte at offset + 51 ignored

            OrderExecutedAtPrice order_executed_at_price(timestamp, orderId, executedQuantity, price,
                                                         executionId, contraOrderId, contraPid, executionType);
            order_executed_at_price.setSymbolMap(&symbol_map);
            order_executed_at_price.payload.assign(data + offset, data + offset + MESSAGE_SIZE);
            return order_executed_at_price;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "OrderExecutedAtPrice{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", executedQuantity=" << executedQuantity
                    << ", executionId=" << executionId
                    << ", contraOrderId=" << contraOrderId
                    << ", contraPid='" << contraPid << "'"
                    << ", executionType='" << executionType << "'"
                    << ", price=" << price << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        double getPrice() const { return price; }
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getContraPID() const { return contraPid; }
        char getExecutionType() const { return executionType; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t executedQuantity;
        double price;
        uint64_t executionId;
        uint64_t contraOrderId;
        std::string contraPid;
        char executionType;

        OrderExecutedAtPrice(uint64_t ts, uint64_t ordId, uint32_t qty, double prc,
                             uint64_t execId, uint64_t contraId, const std::string &contraP,
                             char execType)
            : timestamp(ts), orderId(ordId), executedQuantity(qty), price(prc),
              executionId(execId), contraOrderId(contraId), contraPid(contraP),
              executionType(execType) {
        }
    };
} // namespace CboePitch

#endif // ORDER_EXECUTED_AT_PRICE_H
