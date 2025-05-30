//
// Created by phamanhtan on 30/5/25.
//

#ifndef REDUCE_SIZE_H
#define REDUCE_SIZE_H

#include "message.h"

namespace CboePitch {
    class ReduceSize : public Message {
    public:
        static ReduceSize parse(const uint8_t *data, size_t length);

        std::string toString() const override;

        uint64_t getTimestamp() const { return timestamp; }
        uint64_t getOrderId() const { return orderId; }
        uint32_t getCancelledQuantity() const { return cancelledQuantity; }

    private:
        uint64_t timestamp;
        uint64_t orderId;
        uint32_t cancelledQuantity;

        ReduceSize(uint64_t ts, uint64_t ordId, uint32_t qty)
            : Message(0x39), timestamp(ts), orderId(ordId), cancelledQuantity(qty) {
        }
    };
} // namespace CboePitch

#endif // REDUCE_SIZE_H
