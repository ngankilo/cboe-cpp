#ifndef REDUCE_SIZE_H
#define REDUCE_SIZE_H

#include "message.h"
#include <string>
#include <sstream>
#include <stdexcept>

namespace CboePitch {
    class ReduceSize : public Message {
    public:
        static constexpr uint8_t MESSAGE_TYPE = 0x39;
        static constexpr size_t MESSAGE_SIZE = 22;

        static ReduceSize parse(const uint8_t *data, size_t size, size_t offset = 0) {
            if (size < MESSAGE_SIZE) {
                throw std::invalid_argument("ReduceSize message too short");
            }

            uint64_t timestamp = Message::readUint64LE(data + offset + 2);
            uint64_t orderId = Message::readUint64LE(data + offset + 10);
            uint32_t cancelledQuantity = Message::readUint32LE(data + offset + 18);

            return ReduceSize(timestamp, orderId, cancelledQuantity);
        }

        std::string toString() const override {
            std::ostringstream oss;
            oss << "ReduceSize{timestamp=" << timestamp
                    << ", orderId=" << orderId
                    << ", cancelledQuantity=" << cancelledQuantity << "}";
            return oss.str();
        }

        size_t getMessageSize() const override { return MESSAGE_SIZE; }
        uint8_t getMessageType() const override { return MESSAGE_TYPE; }

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getCancelledQuantity() const { return cancelledQuantity; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t cancelledQuantity;

        ReduceSize(uint64_t ts, uint64_t ordId, uint32_t cancelQty)
            : timestamp(ts), orderId(ordId), cancelledQuantity(cancelQty) {
        }
    };
} // namespace CboePitch

#endif // REDUCE_SIZE_H
