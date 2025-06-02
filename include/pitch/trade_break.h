#ifndef TRADE_BREAK_MESSAGE_H
#define TRADE_BREAK_MESSAGE_H

#include "message.h"
#include <string>
#include <sstream>
#include <stdexcept>

namespace CboePitch {
    class TradeBreak : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x3E;
        static constexpr size_t MESSAGE_SIZE = 18;

        static TradeBreak parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < offset + MESSAGE_SIZE) {
                throw std::invalid_argument("Trade Break message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t executionId = Message::readUint64LE(data + offset + 10);

            return TradeBreak(timestamp, executionId);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "TradeBreak{timestamp=" << timestamp
                    << ", executionId=" << executionId << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getExecutionId() const { return executionId; }

    private:
        uint64_t timestamp;
        uint64_t executionId;

        TradeBreak(uint64_t ts, uint64_t execId)
            : timestamp(ts), executionId(execId) {
        }
    };
} // namespace CboePitch

#endif // TRADE_BREAK_MESSAGE_H
