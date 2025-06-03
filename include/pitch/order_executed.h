#ifndef ORDER_EXECUTED_H
#define ORDER_EXECUTED_H

#include "message.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace CboePitch {
    class OrderExecuted : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x38;
        static constexpr size_t MESSAGE_SIZE = 43;

        static OrderExecuted parse(const uint8_t *data, size_t size, equix_md::SymbolIdentifier &symbol_map,
                                   size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("OrderExecuted message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t executedQuantity = Message::readUint32LE(data + offset + 18);
            uint64_t executionId = Message::readUint64LE(data + offset + 22);
            uint64_t contraOrderId = Message::readUint64LE(data + offset + 30);

            // Contra PID is 4 bytes ASCII, right padded with spaces
            std::string contraPID(reinterpret_cast<const char *>(data + offset + 38), 4);
            // Trim trailing spaces
            contraPID.erase(contraPID.find_last_not_of(' ') + 1);

            OrderExecuted order_executed(timestamp, orderId, executedQuantity, executionId, contraOrderId, contraPID);
            order_executed.setSymbolMap(&symbol_map);
            order_executed.payload.assign(data + offset, data + offset + MESSAGE_SIZE);
            return order_executed;
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "OrderExecuted{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", executedQuantity=" << executedQuantity
                    << ", executionId=" << executionId
                    << ", contraOrderId=" << contraOrderId
                    << ", contraPID='" << contraPID << "'}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        // Accessors
        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getExecutedQuantity() const { return executedQuantity; }
        uint64_t getExecutionId() const { return executionId; }
        uint64_t getContraOrderId() const { return contraOrderId; }
        std::string getContraPID() const { return contraPID; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t executedQuantity;
        uint64_t executionId;
        uint64_t contraOrderId;
        std::string contraPID;

        OrderExecuted(uint64_t ts, uint64_t ordId, uint32_t qty, uint64_t execId,
                      uint64_t contraOrdId, std::string pid)
            : timestamp(ts), orderId(ordId), executedQuantity(qty), executionId(execId),
              contraOrderId(contraOrdId), contraPID(std::move(pid)) {
        }
    };
} // namespace CboePitch

#endif // ORDER_EXECUTED_H
